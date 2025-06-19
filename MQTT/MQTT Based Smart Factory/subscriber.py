# Callback function for when the client connects to the broker
import paho.mqtt.client as mqtt
import json
import argparse
import time

# ANSI color codes for highlighting output
class Colors:
    RED = '\033[91m'
    YELLOW = '\033[93m'
    GREEN = '\033[92m'
    BLUE = '\033[94m'
    ENDC = '\033[0m' # Resets the color

def on_connect(client, userdata, flags, rc):
    """Callback for when the client connects to the broker."""
    if rc == 0:
        print(f"{Colors.GREEN}Connected to MQTT Broker successfully!{Colors.ENDC}")
        # Subscribe to all topics under the 'factory' hierarchy
        # The '#' is a multi-level wildcard.
        topic = "factory/#"
        client.subscribe(topic)
        print(f"Subscribed to topic: '{topic}'")
    else:
        print(f"{Colors.RED}Failed to connect, return code {rc}{Colors.ENDC}\n")

def on_message(client, userdata, msg):
    """Callback for when a message is received."""
    
    try:
        # Decode the payload from bytes to a string, then parse as JSON
        payload_data = json.loads(msg.payload.decode())
        
        # Check if the message is an alarm
        if "alarms" in msg.topic:
            print(f"\n{Colors.RED}--- CRITICAL ALARM RECEIVED ---{Colors.ENDC}")
            print(f"Topic: {msg.topic}")
            print(json.dumps(payload_data, indent=4))
            print(f"{Colors.RED}------------------------------{Colors.ENDC}\n")
        
        # Check if the message is from an environmental sensor
        elif "environment" in msg.topic:
            print(f"\n{Colors.BLUE}--- Environmental Data ---{Colors.ENDC}")
            print(f"Topic: {msg.topic}")
            print(json.dumps(payload_data, indent=4))
            print(f"{Colors.BLUE}--------------------------{Colors.ENDC}\n")

        # Otherwise, it's regular machine telemetry
        else:
            status = payload_data.get("status", "unknown")
            machine_id = payload_data.get("machine_id", "unknown_machine")
            
            # Color-code the output based on machine status
            color = Colors.GREEN if status == "running" else Colors.YELLOW
            if status == "error":
                color = Colors.RED

            print(f"{color}Received telemetry from: {machine_id} | Status: {status}{Colors.ENDC}")
            print(f"Topic: {msg.topic}")
            print(json.dumps(payload_data, indent=2))
            print("-" * 30)

    except json.JSONDecodeError:
        print(f"{Colors.YELLOW}Received non-JSON message on topic {msg.topic}:{Colors.ENDC}")
        print(msg.payload.decode())
    except Exception as e:
        print(f"{Colors.RED}An error occurred: {e}{Colors.ENDC}")

def main(broker, port, user, password):
    """Main function to set up and run the MQTT subscriber."""
    
    subscriber = mqtt.Client()
    subscriber.on_connect = on_connect
    subscriber.on_message = on_message
    subscriber.username_pw_set(user, password)

    try:
        subscriber.connect(broker, port, 60)
    except Exception as e:
        print(f"{Colors.RED}Error: Could not connect to broker. {e}{Colors.ENDC}")
        return

    # loop_forever() is a blocking call that handles reconnection automatically
    # and processes messages. It's ideal for a dedicated subscriber script.
    try:
        print("Starting subscriber... Press Ctrl+C to stop.")
        subscriber.loop_forever()
    except KeyboardInterrupt:
        print("\nSubscriber is stopping.")
    finally:
        subscriber.disconnect()
        print("Disconnected from broker.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="MQTT Factory Subscriber")
    # Set defaults to match the publisher script
    parser.add_argument('--broker', default="192.168.1.105", help="MQTT broker address")
    parser.add_argument('--port', type=int, default=1884, help="MQTT broker port")
    parser.add_argument('--user', default="acm_iot", help="MQTT username")
    parser.add_argument('--password', default="iot", help="MQTT password")
    args = parser.parse_args()

    main(args.broker, args.port, args.user, args.password)
