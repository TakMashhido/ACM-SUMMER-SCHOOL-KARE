import paho.mqtt.client as mqtt
import json
import pandas as pd
from datetime import datetime
import os

# --- MQTT Broker Configuration ---
MQTT_BROKER = "10.10.42.92"
MQTT_PORT = 1884
MQTT_TOPIC = "weather/data"

# --- Add your MQTT credentials here ---
MQTT_USER = "acm_iot"      # Your MQTT username
MQTT_PASS = "iot"      # Your MQTT password

# --- Data Storage Configuration ---
CSV_FILE = "Projects\Fog Computing-Based Weather Forecasting with NodeMCU, Raspberry Pi, and LSTM\Data Collection\weather_data.csv"

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(MQTT_TOPIC)
    elif rc == 5:
        print("Connection refused - not authorised. Check username and password.")
    else:
        print(f"Failed to connect, return code {rc}\n")

def on_message(client, userdata, msg):
    """ The callback for when a PUBLISH message is received from the server."""
    try:
        payload = json.loads(msg.payload.decode())
        temperature = payload.get("temperature")
        humidity = payload.get("humidity")
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        print(f"Received: Temp={temperature}°C, Hum={humidity}% at {timestamp}")

        new_data = pd.DataFrame([[timestamp, temperature, humidity]], columns=['timestamp', 'temperature', 'humidity'])
        
        # Check if file exists to decide on writing header
        if not os.path.isfile(CSV_FILE):
            new_data.to_csv(CSV_FILE, index=False)
        else:
            new_data.to_csv(CSV_FILE, mode='a', header=False, index=False)

    except json.JSONDecodeError:
        print("Error decoding JSON payload")
    except Exception as e:
        print(f"An error occurred: {e}")

# Create the MQTT client
client = mqtt.Client()

# --- Set username and password for authentication ---
client.username_pw_set(MQTT_USER, MQTT_PASS)

# Assign callbacks
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker
try:
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
except Exception as e:
    print(f"Error connecting to broker: {e}")

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
client.loop_forever()