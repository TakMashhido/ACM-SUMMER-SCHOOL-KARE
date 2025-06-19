import paho.mqtt.client as mqtt
import time
import json
import random
import threading
import argparse

# --- Configuration ---
STATUS_OPTIONS = ["running", "idle", "maintenance_required", "error"]
ERROR_CODES = [
    "E-101-MOTOR-FAIL", 
    "E-203-SENSOR-DISCONNECT", 
    "E-404-NETWORK-TIMEOUT", 
    "E-500-OVERHEAT"
]

class Machine:
    """Represents a single stateful machine in the factory."""
    
    def __init__(self, machine_id, topic):
        self.id = machine_id
        self.topic = topic
        self.state = "idle"
        self.production_cycles = 0
        self.cycles_until_maintenance = random.randint(50, 100)

    def update_state(self):
        """Simulates state transitions for the machine."""
        if self.state == "running":
            self.production_cycles += 1
            if self.production_cycles >= self.cycles_until_maintenance:
                self.state = "maintenance_required"
                self.production_cycles = 0
            elif random.random() < 0.01: # 1% chance of random error
                self.state = "error"
        
        elif self.state == "idle":
            if random.random() < 0.6: # 60% chance to start running
                self.state = "running"

        elif self.state in ["maintenance_required", "error"]:
            if random.random() < 0.3: # 30% chance of being fixed
                self.state = "idle"

    def generate_telemetry(self):
        """Generates realistic telemetry based on the machine's current state."""
        self.update_state()
        timestamp = time.time()
        base_data = {"timestamp": timestamp, "machine_id": self.id, "status": self.state}

        if self.state == "running":
            if "cnc" in self.id:
                base_data.update({
                    "spindle_speed_rpm": random.uniform(1200, 1800),
                    "tool_temp_c": random.uniform(50.0, 70.0),
                    "vibration_g": random.uniform(0.5, 1.2)
                })
            elif "robot" in self.id:
                 base_data.update({
                    "position_error_mm": random.uniform(0.05, 0.15),
                    "joint_temps_c": [random.uniform(60, 80) for _ in range(6)],
                    "cycle_time_s": random.uniform(6.0, 7.5)
                })
        
        elif self.state == "error":
            base_data["error_code"] = random.choice(ERROR_CODES)
            # Simulate abnormal readings during an error
            if "cnc" in self.id:
                base_data["spindle_speed_rpm"] = 0
                base_data["vibration_g"] = random.uniform(2.0, 5.0)

        return base_data, self.state == "error"

class EnvironmentalSensor:
    """Represents an environmental sensor for a factory zone."""
    def __init__(self, sensor_id, topic):
        self.id = sensor_id
        self.topic = topic

    def generate_telemetry(self):
        return {
            "timestamp": time.time(),
            "sensor_id": self.id,
            "temperature_c": random.uniform(22.0, 28.0),
            "humidity_percent": random.uniform(40.0, 55.0),
            "air_quality_ppm": random.randint(50, 150)
        }

class FactorySimulator:
    """Manages all machines, sensors, and the MQTT connection."""

    def __init__(self, broker, port, user, password):
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.username_pw_set(user, password)
        
        print(f"Connecting to broker at {broker}:{port}...")
        try:
            self.client.connect(broker, port, 60)
        except Exception as e:
            print(f"Error: Could not connect to broker. {e}")
            exit()

        self.machines = [
            Machine("cnc_machine_01", "factory/floor_1/cnc_01/telemetry"),
            Machine("welding_robot_01", "factory/floor_1/robot_01/telemetry"),
            Machine("packaging_station_01", "factory/floor_2/packaging_01/telemetry"),
        ]
        self.env_sensors = [
            EnvironmentalSensor("env_sensor_floor_1", "factory/floor_1/environment"),
            EnvironmentalSensor("env_sensor_floor_2", "factory/floor_2/environment"),
        ]
        self.alarm_topic = "factory/events/alarms"
    
    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker successfully!")
        else:
            print(f"Failed to connect, return code {rc}\n")

    def publish_data(self, topic, payload):
        """Publishes a JSON payload to a specific topic."""
        json_payload = json.dumps(payload, indent=4)
        result = self.client.publish(topic, json_payload)
        if result[0] != 0:
            print(f"Failed to send message to topic {topic}")

    def run_simulation(self):
        """Main simulation loop."""
        self.client.loop_start()
        print("Starting factory data simulator... Press Ctrl+C to stop.")
        
        try:
            while True:
                # Simulate machine telemetry
                for machine in self.machines:
                    telemetry, is_error = machine.generate_telemetry()
                    self.publish_data(machine.topic, telemetry)
                    
                    # If an error occurred, publish an alarm
                    if is_error:
                        alarm_payload = {
                            "timestamp": telemetry["timestamp"],
                            "machine_id": machine.id,
                            "error_code": telemetry["error_code"],
                            "severity": "high"
                        }
                        self.publish_data(self.alarm_topic, alarm_payload)
                        print(f"*** ALARM PUBLISHED for {machine.id} ***")

                    time.sleep(random.uniform(1.0, 3.0))

                # Simulate environmental data less frequently
                if random.random() < 0.25:
                    for sensor in self.env_sensors:
                        telemetry = sensor.generate_telemetry()
                        self.publish_data(sensor.topic, telemetry)
                    print("--- Published environmental data ---")

                print("--- Cycle complete, waiting... ---")
                time.sleep(5)

        except KeyboardInterrupt:
            print("\nSimulator is stopping.")
        finally:
            self.client.loop_stop()
            self.client.disconnect()
            print("Disconnected from broker.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="MQTT Factory Simulator")
    parser.add_argument('--broker', default="192.168.1.105", help="MQTT broker address")
    parser.add_argument('--port', type=int, default=1884, help="MQTT broker port")
    parser.add_argument('--user', default="acm_iot", help="MQTT username")
    parser.add_argument('--password', default="iot", help="MQTT password")
    args = parser.parse_args()

    simulator = FactorySimulator(args.broker, args.port, args.user, args.password)
    simulator.run_simulation()
# This code simulates a factory environment with machines and sensors, publishing telemetry data to an MQTT broker.
# It includes realistic state transitions, error handling, and environmental monitoring.    