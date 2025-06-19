import paho.mqtt.client as mqtt

# 1. --- Define MQTT Broker, Topic, and Authentication ---
# These must be the SAME as the publisher's configuration.
BROKER_ADDRESS = "192.168.1.105"
MQTT_PORT = 1884
TOPIC = "iot/class/test" 

# --- Add your authentication credentials here ---
MQTT_USERNAME = "acm_iot"
MQTT_PASSWORD = "iot"
# -----------------------------------------------

# 2. --- Define Callback Functions ---
# These functions run automatically when specific MQTT events happen.

# This function runs when the client successfully connects to the broker.
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker successfully!")
        # Once connected, we subscribe to our topic.
        client.subscribe(TOPIC)
        print(f"Subscribed to topic: '{TOPIC}'")
    else:
        # rc=4 means Bad Username or Password. rc=5 means Not Authorized.
        print(f"Failed to connect, return code {rc}")

# This function runs whenever a message is received on a subscribed topic.
def on_message(client, userdata, msg):
    # msg.payload is the message content in bytes, so we decode it to a string.
    print(f"Received message: '{msg.payload.decode()}' on topic '{msg.topic}'")

# 3. --- Create an MQTT Client and Assign Callbacks ---
client = mqtt.Client()

# Set username and password before connecting
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

client.on_connect = on_connect   # Assign the on_connect function
client.on_message = on_message   # Assign the on_message function

# 4. --- Connect to the Broker ---
print(f"Connecting to broker at {BROKER_ADDRESS}...")
try:
    client.connect(BROKER_ADDRESS, MQTT_PORT)
except Exception as e:
    print(f"Error connecting to broker: {e}")
    exit()


# 5. --- Start the Client Loop ---
# loop_forever() is a blocking function that keeps the script running
# to listen for incoming messages. It also handles auto-reconnection.
print("Waiting for messages... Press Ctrl+C to stop.")
try:
    client.loop_forever()
except KeyboardInterrupt:
    print("\nSubscriber is stopping.")
    client.disconnect()
    print("Disconnected.")
