import asyncio
import struct
from bleak import BleakClient, BleakScanner
from playsound import playsound
import threading

# Match the exact name you gave your ESP32 device
DEVICE_NAME = "Auslan_glove"

# These must match the UUIDs in your ESP32 code
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# Play sound in a separate thread to avoid blocking BLE notifications
def play_sound(filename):
    threading.Thread(target=playsound, args=(filename,), daemon=True).start()

async def main():
    print("Scanning for ESP32...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)

    if not device:
        print("Device not found. Is it powered on and advertising?")
        return

    print(f"Found {DEVICE_NAME} at {device.address}")
    
    async with BleakClient(device) as client:
        print("Connected. Subscribing to notifications...")

        last_fingers = {}

        def notification_handler(sender, data):
            ax, ay, az, flex_byte, heartbeat = struct.unpack('<hhhBB', data)

            fingers = {
                "Thumb":   (flex_byte >> 0) & 1,
                "Index":   (flex_byte >> 1) & 1,
                "Middle":  (flex_byte >> 2) & 1,
                "Ring":    (flex_byte >> 3) & 1,
                "Pinky":   (flex_byte >> 4) & 1,
            }

            print(f"Accel: ({ax}, {ay}, {az}) | Fingers: {fingers} | Heartbeat: {heartbeat}")

            # Example: Play a sound when a finger bends (value goes from 0 -> 1)
            for finger, state in fingers.items():
                if last_fingers.get(finger) != state and state == 1:
                    sound_file = f"sounds/{finger.lower()}.mp3"
                    print(f"Playing sound for {finger}")
                    play_sound(sound_file)

            # Heartbeat can trigger a different sound
            if heartbeat > 180:
                play_sound("sounds/heartbeat_alert.mp3")

            # Save current finger state
            last_fingers.update(fingers)

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        while True:
            await asyncio.sleep(1)

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("Disconnected.")

asyncio.run(main())
