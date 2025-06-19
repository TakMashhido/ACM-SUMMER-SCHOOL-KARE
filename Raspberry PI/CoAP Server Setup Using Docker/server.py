import logging
import asyncio
from aiocoap import *
import aiocoap.resource as resource

# --- The Resource (The "Smart Light" functionality) ---

class LightResource(resource.Resource):
    """
    This class represents our smart light. It has two attributes:
    - self.is_on: A boolean (True/False) for the light's state.
    - self.brightness: An integer (0-100) for the brightness level.
    """
    def __init__(self):
        super().__init__()
        self.is_on = False
        self.brightness = 100

    async def render_get(self, request):
        """
        This method is called when the server receives a GET request.
        It reads the state of the light and sends it back to the client.
        """
        payload_str = f"State: {'On' if self.is_on else 'Off'}, Brightness: {self.brightness}%"
        print(f"Responding to GET request: {payload_str}")
        
        # We encode the string into bytes to send it.
        payload = payload_str.encode('utf-8')
        
        # The response is sent with a "Content" code (like HTTP 200 OK).
        return Message(payload=payload, code=CONTENT)

    async def render_put(self, request):
        """
        This method is called when the server receives a PUT request.
        It reads the command from the client's payload and updates the light's state.
        """
        payload_str = request.payload.decode('utf-8').lower()
        print(f"Received PUT request with payload: {payload_str}")
        
        # --- Simple command processing ---
        if "on" in payload_str:
            self.is_on = True
            print("Action: Turned light ON")
        elif "off" in payload_str:
            self.is_on = False
            print("Action: Turned light OFF")
        
        # Check for brightness commands, e.g., "brightness=80"
        if "brightness=" in payload_str:
            try:
                # Extract the value after "brightness="
                new_brightness = int(payload_str.split('brightness=')[1])
                if 0 <= new_brightness <= 100:
                    self.brightness = new_brightness
                    print(f"Action: Set brightness to {self.brightness}%")
                else:
                    # If value is invalid, send a "Bad Request" error code.
                    return Message(code=BAD_REQUEST, payload="Brightness must be 0-100".encode())
            except ValueError:
                return Message(code=BAD_REQUEST, payload="Invalid brightness value".encode())
        
        # A successful PUT request responds with a "Changed" code.
        return Message(code=CHANGED)

# --- Main Server Setup ---
logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.INFO)

async def main():
    # Create the root resource which will hold all other resources.
    root = resource.Site()
    
    # Add our LightResource to the path '/light'.
    # This means clients will access it at coap://<server-ip>/light
    root.add_resource(['light'], LightResource())

    print("CoAP server running. Listening on all interfaces.")
    print("Access the resource at: coap://localhost/light")
    
    # Bind the server to all available network interfaces (IPv6 and IPv4).
    await Context.create_server_context(root)

    # Keep the server running until it's manually stopped (e.g., with Ctrl+C).
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nServer stopped.")
