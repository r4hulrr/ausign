# esp32_ble_decoder.py

import asyncio
from bleak import BleakScanner, BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def find_glove_ble_device():
    print("🔍 Scanning for BLE devices...")
    devices = await BleakScanner.discover(timeout=5.0)
    for d in devices:
        # Optional: filter by name or service UUID
        if d.name and "GloveBLE" in d.name:
            print(f"✅ Found GloveBLE: {d.name} [{d.address}]")
            return d.address
    print("❌ GloveBLE device not found.")
    return None

async def read_characteristic(address):
    print(f"🔗 Connecting to {address}...")
    async with BleakClient(address) as client:
        if await client.is_connected():
            print("📡 Connected.")
            try:
                value = await client.read_gatt_char(CHARACTERISTIC_UUID)
                decoded = value.decode('utf-8')
                print(f"📨 Received: {decoded}")
                return decoded
            except Exception as e:
                print(f"❌ Error reading characteristic: {e}")
        else:
            print("❌ Failed to connect.")
    return None

async def main():
    address = await find_glove_ble_device()
    if address:
        await read_characteristic(address)

if __name__ == "__main__":
    asyncio.run(main())
