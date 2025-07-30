import asyncio
import struct
from bleak import BleakClient, BleakScanner
from playsound import playsound
import threading

DEVICE_NAME = "Auslan_glove"
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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
        last_ax = last_ay = last_az = None
        last_detected_sign = None  # To prevent repeated plays

        def notification_handler(sender, data):
            nonlocal last_ax, last_ay, last_az, last_fingers, last_detected_sign

            ax, ay, az, flex_byte, heartbeat = struct.unpack('<hhhBB', data)

            fingers = {
                "Thumb":   (flex_byte >> 0) & 1,
                "Index":   (flex_byte >> 1) & 1,
                "Middle":  (flex_byte >> 2) & 1,
                "Ring":    (flex_byte >> 3) & 1,
                "Pinky":   (flex_byte >> 4) & 1,
            }

            print(f"Accel: ({ax}, {ay}, {az}) | Fingers: {fingers} | Heartbeat: {heartbeat}")

            # --- Define sign mappings ---
            detected_sign = None

            # Example: "Hello" sign
            if (
                fingers["Thumb"] == 0 and
                fingers["Index"] == 1 and
                fingers["Middle"] == 1 and
                fingers["Ring"] == 1 and
                fingers["Pinky"] == 1 and
                ax > 800  # Palm upright
            ):
                detected_sign = "hello"

            # Example: "Yes" sign
            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 0 and
                fingers["Pinky"] == 0 and
                -220 < ay < -180  # Palm sideways
            ):
                detected_sign = "yes"

            # Example: "No" sign
            elif (
                fingers["Thumb"] == 0 and
                fingers["Index"] == 1 and
                fingers["Middle"] == 1 and
                fingers["Ring"] == 0 and
                fingers["Pinky"] == 0 and
                az < -80 and ay > 300  # Palm toward face
            ):
                detected_sign = "no"

            # --- Play only if sign changed ---
            if detected_sign and detected_sign != last_detected_sign:
                print(f"Detected sign: {detected_sign}")
                last_detected_sign = detected_sign

            # Clear sign if gesture disappears
            elif detected_sign is None:
                last_detected_sign = None

            # Update history
            last_fingers.update(fingers)
            last_ax = ax
            last_ay = ay
            last_az = az

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        while True:
            await asyncio.sleep(1)

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("Disconnected.")

asyncio.run(main())
