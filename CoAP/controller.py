import asyncio
from aiocoap import *

uri = "coap://192.168.1.105/light"

async def main():
    """A simple client script to interact with the CoAP smart light server."""
    
    # Create a CoAP client context.
    protocol = await Context.create_client_context()
    
    print("--- CoAP Client Demo ---")

    # --- 1. First GET request: Check initial state ---
    print("\n[1] Sending GET request to check the light's initial state...")
    get_request_1 = Message(code=GET, uri=uri)
    try:
        response_1 = await protocol.request(get_request_1).response
        print(f"    > Server Response: {response_1.payload.decode('utf-8')}")
    except Exception as e:
        print(f"Failed to get response: {e}")
    
    await asyncio.sleep(1)

    # --- 2. First PUT request: Turn the light ON ---
    print("\n[2] Sending PUT request to turn the light ON...")
    put_request_1 = Message(code=PUT, uri=uri, payload="on".encode('utf-8'))
    try:
        response_2 = await protocol.request(put_request_1).response
        print(f"    > Server acknowledged change with code: {response_2.code}")
    except Exception as e:
        print(f"Failed to send request: {e}")

    await asyncio.sleep(1)

    # --- 3. Second GET request: Check the new state ---
    print("\n[3] Sending GET request to confirm the light is on...")
    get_request_2 = Message(code=GET, uri=uri)
    try:
        response_3 = await protocol.request(get_request_2).response
        print(f"    > Server Response: {response_3.payload.decode('utf-8')}")
    except Exception as e:
        print(f"Failed to get response: {e}")

    await asyncio.sleep(1)

    # --- 4. Second PUT request: Change brightness ---
    print("\n[4] Sending PUT request to set brightness to 75%...")
    put_request_2 = Message(code=PUT, uri=uri, payload="brightness=75".encode('utf-8'))
    try:
        response_4 = await protocol.request(put_request_2).response
        print(f"    > Server acknowledged change with code: {response_4.code}")
    except Exception as e:
        print(f"Failed to send request: {e}")
        
    await asyncio.sleep(1)
        
    # --- 5. Final GET request: Check final state ---
    print("\n[5] Sending final GET request to see final state...")
    get_request_3 = Message(code=GET, uri=uri)
    try:
        response_5 = await protocol.request(get_request_3).response
        print(f"    > Server Response: {response_5.payload.decode('utf-8')}")
    except Exception as e:
        print(f"Failed to get response: {e}")


if __name__ == "__main__":
    asyncio.run(main())
