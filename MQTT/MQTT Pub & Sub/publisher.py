import paho.mqtt.client as mqtt
import time

# 1. --- Define MQTT Broker, Topic, and Authentication ---
# We are using a public broker that supports authentication.
BROKER_ADDRESS = "192.168.1.105"
MQTT_PORT = 1884
TOPIC = "iot/class/test" 

# --- Add your authentication credentials here ---
# These must match the credentials on your MQTT broker.
MQTT_USERNAME = "acm_iot"
MQTT_PASSWORD = "iot"
# -----------------------------------------------

# 2. --- Create an MQTT Client ---
client = mqtt.Client()

# 3. --- Set Username and Password ---
# This function tells the client to use these credentials for authentication.
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

# 4. --- Connect to the Broker ---
# We connect to the broker using its address and port.
print(f"Connecting to broker at {BROKER_ADDRESS}...")
try:
    client.connect(BROKER_ADDRESS, MQTT_PORT)
except Exception as e:
    print(f"Error connecting to broker: {e}")
    exit()

print("Connected!")
client.loop_start() # Handles reconnections automatically

# 5. --- Start Publishing Messages ---
# We'll use a loop to send a message every 3 seconds.
try:
    message_count = 0
    while True:
        message_count += 1
        # The message we want to send.
        message = f"Hello from the authenticated publisher! Message #{message_count}"
        
        # The publish() function sends the message.
        result = client.publish(TOPIC, message)
        
        # Check if the publish was successful
        status = result[0]
        if status == 0:
            print(f"Sent: '{message}' to topic '{TOPIC}'")
        else:
            print(f"Failed to send message to topic {TOPIC}")

        time.sleep(3)

# This part handles you pressing Ctrl+C to stop the script
except KeyboardInterrupt:
    print("\nPublisher is stopping.")
finally:
    # 6. --- Disconnect from the Broker ---
    client.loop_stop()
    client.disconnect()
    print("Disconnected from broker.")

