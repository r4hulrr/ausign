import asyncio
import struct
import os
from bleak import BleakClient, BleakScanner
import pygame

DEVICE_NAME = "Auslan_glove"
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

audio_initialized = False

def play_sound(filename):
    global audio_initialized
    try:
        if not audio_initialized:
            pygame.mixer.init()
            audio_initialized = True
        pygame.mixer.music.load(filename)
        pygame.mixer.music.play()
    except Exception as e:
        print(f"Error playing sound: {e}")

async def main():
    print("Scanning for ESP32...")
    try:
        device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)
    except asyncio.exceptions.CancelledError:
        print("Scan cancelled unexpectedly. Retrying...")
        return

    if not device:
        print("Device not found. Is it powered on and advertising?")
        return

    print(f"Found {DEVICE_NAME} at {device.address}")
    
    async with BleakClient(device) as client:
        print("Connected. Subscribing to notifications...")

        last_fingers = {}
        last_ax = last_ay = last_az = None
        last_detected_sign = None

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

            detected_sign = None
            sound_file = None

            if (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 1 and
                fingers["Ring"] == 1 and
                fingers["Pinky"] == 1 and
                ax > 800
            ):
                detected_sign = "1"
                sound_file = os.path.abspath("python\one.wav")

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 1 and
                fingers["Pinky"] == 1 and
                ax > 800
            ):
                detected_sign = "2"
                sound_file = os.path.abspath("python/two.wav")

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 0 and
                fingers["Pinky"] == 1 and
                ax > 800
            ):
                detected_sign = "3"
                sound_file = os.path.abspath("python/three.wav")

            if detected_sign and detected_sign != last_detected_sign:
                print(f"Detected sign: {detected_sign}")
                last_detected_sign = detected_sign
                if sound_file:
                    play_sound(sound_file)

                asyncio.create_task(client.write_gatt_char(CHARACTERISTIC_UUID, detected_sign.encode(), response=False))
            elif detected_sign is None:
                last_detected_sign = None

            last_fingers.update(fingers)
            last_ax = ax
            last_ay = ay
            last_az = az

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        try:
            while True:
                await asyncio.sleep(1)
        except asyncio.CancelledError:
            pass

        await client.stop_notify(CHARACTERISTIC_UUID)
        print("Disconnected.")

asyncio.run(main())
