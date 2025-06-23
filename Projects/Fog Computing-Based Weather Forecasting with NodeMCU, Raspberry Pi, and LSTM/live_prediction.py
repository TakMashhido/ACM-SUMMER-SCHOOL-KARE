import paho.mqtt.client as mqtt
import numpy as np
import joblib
import json
import logging
import sys
from collections import deque

import torch
import torch.nn as nn

# --- Setup Structured Logging ---
# This provides clearer, timestamped output than print()
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    stream=sys.stdout
)

# --- Configuration (Match your setup) ---
# MQTT Broker details from your NodeMCU code
MQTT_BROKER_HOST = "10.10.42.92"
MQTT_BROKER_PORT = 1884
MQTT_USERNAME = "acm_iot"
MQTT_PASSWORD = "iot"
MQTT_TOPIC_IN = "weather/data"      # Topic to listen for sensor data
MQTT_TOPIC_OUT = "weather/forecast" # Topic to publish predictions

# Model Configuration (Must match the training script)
LOOK_BACK = 60 # The number of past data points the model needs

# --- PyTorch Model Definition ---
# This class MUST be identical to the one in your training script
# so that PyTorch knows the model's architecture.
class LSTMModel(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers, output_size, dropout_prob):
        super(LSTMModel, self).__init__()
        self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True)
        self.dropout = nn.Dropout(dropout_prob)
        self.fc = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        out, _ = self.lstm(x)
        out = self.dropout(out[:, -1, :]) # Get output from the last time step
        out = self.fc(out)
        return out

# --- Global Variables & Data Buffer ---
# A deque is a highly efficient list-like object with a fixed max length.
# When it's full, adding a new item automatically discards the oldest item.
DATA_BUFFER = deque(maxlen=LOOK_BACK)
MODEL = None
FEATURE_SCALER = None
TARGET_SCALER = None
DEVICE = torch.device("cpu") # Use CPU for inference on Raspberry Pi

def make_prediction():
    """Processes the buffer, runs the model, and returns a single prediction."""
    global MODEL, FEATURE_SCALER, TARGET_SCALER, DEVICE, DATA_BUFFER

    # 1. Convert the buffer of recent data points to a NumPy array
    data_sequence = np.array(list(DATA_BUFFER))

    # 2. Scale the features using the loaded 'feature_scaler'
    scaled_sequence = FEATURE_SCALER.transform(data_sequence)

    # 3. Convert to a PyTorch tensor, add a batch dimension [1, LOOK_BACK, num_features]
    #    and send it to the CPU.
    tensor_sequence = torch.tensor(scaled_sequence, dtype=torch.float32).unsqueeze(0).to(DEVICE)

    # 4. Make a prediction
    MODEL.eval() # Set the model to evaluation mode
    with torch.no_grad(): # Disable gradient calculation for faster inference
        scaled_prediction = MODEL(tensor_sequence)

    # 5. Inverse transform the prediction to get the actual temperature value
    #    using the loaded 'target_scaler'.
    prediction = TARGET_SCALER.inverse_transform(scaled_prediction.cpu().numpy())

    # The prediction is a 2D array, so we extract the single value from it
    return prediction[0][0]

def on_message(client, userdata, msg):
    """Callback function executed every time a message is received on weather/data."""
    try:
        # 1. Decode the incoming JSON data from the NodeMCU
        payload = json.loads(msg.payload.decode())
        temperature = payload.get("temperature")
        humidity = payload.get("humidity")

        if temperature is None or humidity is None:
            logging.warning("Received incomplete data packet. Skipping.")
            return

        # 2. Add the new data point [temp, humidity] to our buffer.
        DATA_BUFFER.append([temperature, humidity])
        logging.info(f"Received: Temp={temperature:.2f}°C, Hum={humidity:.2f}%. Buffer: {len(DATA_BUFFER)}/{LOOK_BACK}")

        # 3. Only make a prediction if the buffer is full (i.e., we have enough data)
        if len(DATA_BUFFER) == LOOK_BACK:
            logging.info("Buffer is full. Running model to get forecast...")
            # 4. Get the forecast
            forecast = make_prediction()
            logging.info(f"--- PREDICTED TEMP: {forecast:.2f}°C ---")

            # 5. Publish the forecast as JSON to the output topic
            forecast_payload = json.dumps({"predicted_temperature": f"{forecast:.2f}"})
            client.publish(MQTT_TOPIC_OUT, forecast_payload)

    except json.JSONDecodeError:
        logging.error("Could not decode JSON from MQTT message. Check NodeMCU payload.")
    except Exception as e:
        logging.error(f"An error occurred in on_message: {e}", exc_info=True)

def on_connect(client, userdata, flags, rc):
    """Callback for when the client connects to the MQTT broker."""
    if rc == 0:
        logging.info("Successfully connected to MQTT Broker!")
        # Subscribe to the input topic after connecting
        client.subscribe(MQTT_TOPIC_IN)
        logging.info(f"Subscribed to topic: '{MQTT_TOPIC_IN}'")
    else:
        logging.error(f"Failed to connect to MQTT Broker, return code {rc}. Check host, port, and credentials.")

def main():
    """Main function to load artifacts and start the MQTT client."""
    global MODEL, FEATURE_SCALER, TARGET_SCALER

    # --- Load Trained Model and Scalers ---
    logging.info("Starting Live Predictor Service...")
    try:
        logging.info("Loading model and scalers...")
        # Load the scalers saved from the training script
        FEATURE_SCALER = joblib.load('feature_scaler.gz')
        TARGET_SCALER = joblib.load('target_scaler.gz')

        # Determine model parameters from the loaded scaler
        num_features = FEATURE_SCALER.n_features_in_
        hidden_size = 50 # Must match the model in the training script
        num_layers = 1
        output_size = 1
        dropout_prob = 0.2

        # Instantiate the model architecture
        MODEL = LSTMModel(num_features, hidden_size, num_layers, output_size, dropout_prob).to(DEVICE)

        # Load the saved model weights
        MODEL.load_state_dict(torch.load('best_short_term_model.pth', map_location=DEVICE))
        logging.info("Model and scalers loaded successfully.")

    except FileNotFoundError as e:
        logging.error(f"Fatal Error: Could not load required file: {e}. Make sure 'best_short_term_model.pth', 'feature_scaler.gz', and 'target_scaler.gz' are in the same directory.")
        return
    except Exception as e:
        logging.error(f"An error occurred during setup: {e}", exc_info=True)
        return

    # --- Initialize MQTT Client ---
    client = mqtt.Client()
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        logging.info(f"Connecting to MQTT broker at {MQTT_BROKER_HOST}:{MQTT_BROKER_PORT}...")
        client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT, 60)
        # loop_forever() is a blocking call that handles all MQTT traffic
        client.loop_forever()
    except ConnectionRefusedError:
        logging.error("Connection refused. Is the broker running? Are the host/port correct?")
    except Exception as e:
        logging.error(f"Could not connect to MQTT broker: {e}", exc_info=True)

if __name__ == "__main__":
    main()