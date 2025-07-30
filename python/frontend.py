import asyncio
import struct
import threading
import tkinter as tk
from bleak import BleakClient, BleakScanner
from playsound import playsound

DEVICE_NAME = "Auslan_glove"
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# -------- GUI SETUP -------- #
root = tk.Tk()
root.title("Auslan Glove Realtime Monitor")
root.geometry("400x300")
root.resizable(False, False)

accel_label = tk.Label(root, text="Accel: (0, 0, 0)", font=("Helvetica", 12))
accel_label.pack(pady=10)

fingers_label = tk.Label(root, text="Fingers: --", font=("Helvetica", 12))
fingers_label.pack(pady=10)

sign_label = tk.Label(root, text="Detected Sign: None", font=("Helvetica", 14, "bold"), fg="blue")
sign_label.pack(pady=20)

status_label = tk.Label(root, text="Connecting...", font=("Helvetica", 10))
status_label.pack(pady=5)

def update_gui(ax, ay, az, fingers, sign):
    accel_label.config(text=f"Accel: ({ax}, {ay}, {az})")
    fingers_text = ', '.join([f"{k}:{v}" for k, v in fingers.items()])
    fingers_label.config(text=f"Fingers: {fingers_text}")
    sign_label.config(text=f"Detected Sign: {sign if sign else 'None'}")

def update_status(text):
    status_label.config(text=text)

def play_sound_async(filename):
    threading.Thread(target=playsound, args=(filename,), daemon=True).start()

# -------- BLE BACKEND -------- #
async def run_ble():
    update_status("Scanning for ESP32...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)

    if not device:
        update_status("Device not found!")
        return

    update_status(f"Found {DEVICE_NAME}. Connecting...")
    async with BleakClient(device) as client:
        update_status("Connected. Waiting for data...")

        last_detected_sign = None

        def notification_handler(sender, data):
            nonlocal last_detected_sign

            ax, ay, az, flex_byte, heartbeat = struct.unpack('<hhhBB', data)
            fingers = {
                "Thumb":   (flex_byte >> 0) & 1,
                "Index":   (flex_byte >> 1) & 1,
                "Middle":  (flex_byte >> 2) & 1,
                "Ring":    (flex_byte >> 3) & 1,
                "Pinky":   (flex_byte >> 4) & 1,
            }

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
                sound_file = "one.mp3"

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 1 and
                fingers["Pinky"] == 1 and
                ax > 800
            ):
                detected_sign = "2"
                sound_file = "two.mp3"

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 0 and
                fingers["Pinky"] == 1 and
                ax > 800
            ):
                detected_sign = "3"
                sound_file = "three.mp3"

            if detected_sign and detected_sign != last_detected_sign:
                play_sound_async(sound_file)
                last_detected_sign = detected_sign
            elif detected_sign is None:
                last_detected_sign = None

            root.after(0, update_gui, ax, ay, az, fingers, detected_sign)

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        while True:
            await asyncio.sleep(1)

# -------- Start GUI with Async BLE -------- #
def start_loop():
    asyncio.run(run_ble())

threading.Thread(target=start_loop, daemon=True).start()
root.mainloop()
