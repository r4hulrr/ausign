import asyncio
from bleak import BleakClient, BleakScanner

# Match the exact name you gave your ESP32 device
DEVICE_NAME = "Auslan_glove"

# These must match the UUIDs in your ESP32 code
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def main():
    print("Scanning for ESP32...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)

    if not device:
        print("Device not found. Is it powered on and advertising?")
        return

    print(f"Found {DEVICE_NAME} at {device.address}")
    
    async with BleakClient(device) as client:
        print("Connected. Subscribing to notifications...")

        def notification_handler(sender, data):
            # data is bytes
            print(f"{data.decode('utf-8')}")

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        while True:
            await asyncio.sleep(1)

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("Disconnected.")

asyncio.run(main())
