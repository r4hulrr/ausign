import asyncio
import struct
import os
import time
import tkinter as tk
from bleak import BleakClient, BleakScanner
import pygame
import threading
from symbols import sign_definitions  # Loaded from CSV

# BLE Settings
DEVICE_NAME = "Auslan_glove"
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# Audio
audio_initialized = False
is_muted = False

def play_sound(filename):
    global audio_initialized, is_muted
    if is_muted or not filename or not os.path.isfile(filename):
        return
    try:
        if not audio_initialized:
            pygame.mixer.init()
            audio_initialized = True
        sound = pygame.mixer.Sound(filename)
        sound.play()
    except Exception as e:
        print(f"Error playing sound: {e}")

def delayed_play(filename, delay=0.3):
    def _delayed():
        time.sleep(delay)
        play_sound(filename)
    threading.Thread(target=_delayed, daemon=True).start()

# GUI Setup
root = tk.Tk()
root.title("Auslan Glove Monitor")
root.geometry("400x320")
root.resizable(False, False)

accel_label = tk.Label(root, text="Accel: (0, 0, 0)", font=("Helvetica", 12))
accel_label.pack(pady=5)

fingers_frame = tk.Frame(root)
fingers_frame.pack(pady=5)

finger_labels = {}
for finger in ["Thumb", "Index", "Middle", "Ring", "Little"]:
    frame = tk.Frame(fingers_frame)
    frame.pack(anchor='w')
    label = tk.Label(frame, text=f"{finger}: 0", font=("Helvetica", 12))
    label.pack(side='left')
    finger_labels[finger] = label

heartbeat_label = tk.Label(root, text="Heartbeat: --", font=("Helvetica", 12))
heartbeat_label.pack(pady=5)

sign_label = tk.Label(root, text="Detected Sign: None", font=("Helvetica", 14, "bold"), fg="blue")
sign_label.pack(pady=10)

def toggle_mute():
    global is_muted
    is_muted = not is_muted
    mute_button.config(text="Unmute" if is_muted else "Mute")

mute_button = tk.Button(root, text="Mute", command=toggle_mute)
mute_button.pack(pady=5)

status_label = tk.Label(root, text="Bluetooth Status: Scanning...", font=("Helvetica", 10), fg="gray")
status_label.pack(pady=10)

# BLE Loop Thread
def run_ble_loop():
    asyncio.run(main())

def update_gui(ax, ay, az, fingers, heartbeat, detected_sign, ble_status=None):
    accel_label.config(text=f"Accel: ({ax}, {ay}, {az})")
    for finger, value in fingers.items():
        finger_labels[finger].config(text=f"{finger}: {value}")
    heartbeat_label.config(text=f"Heartbeat: {heartbeat}")
    sign_label.config(text=f"Detected Sign: {detected_sign}")
    if ble_status is not None:
        status_label.config(text=f"Bluetooth Status: {ble_status}")

async def main():
    root.after(0, update_gui, 0, 0, 0, {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0}, "--", "None", "Scanning...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)

    if not device:
        root.after(0, update_gui, 0, 0, 0, {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0}, "--", "None", "Device not found.")
        return

    root.after(0, update_gui, 0, 0, 0, {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0}, "--", "None", f"Found {DEVICE_NAME}, connecting...")

    async with BleakClient(device) as client:
        root.after(0, update_gui, 0, 0, 0, {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0}, "--", "None", "Connected")

        last_detected_sign = "None"
        previous_sign = "None"
        candidate_sign = None
        candidate_count = 0

        def notification_handler(sender, data):
            nonlocal last_detected_sign, previous_sign, candidate_sign, candidate_count

            ax, ay, az, flex_byte, heartbeat = struct.unpack('<hhhBB', data)

            fingers = {
                "Thumb":  (flex_byte >> 0) & 1,
                "Index":  (flex_byte >> 1) & 1,
                "Middle": (flex_byte >> 2) & 1,
                "Ring":   (flex_byte >> 3) & 1,
                "Little": (flex_byte >> 4) & 1,
            }

            detected_sign = None
            sound_file = None

            for symbol in sign_definitions:
                match = all(fingers[f] == v for f, v in symbol["fingers"].items())
                if match and symbol["conditions"](ax, ay, az, heartbeat):
                    detected_sign = symbol["sign"]
                    sound_file = os.path.abspath(symbol["audio"]) if symbol["audio"] else None
                    break

            if detected_sign:
                if detected_sign == candidate_sign:
                    candidate_count += 1
                else:
                    candidate_sign = detected_sign
                    candidate_count = 1

                if detected_sign != last_detected_sign:
                    # Combo: "This" -> "My"
                    if previous_sign == "This" and detected_sign == "My":
                        play_sound("sounds/is.wav")
                        delayed_play(sound_file, delay=0.3)
                    # Combo: "My" -> "Name"
                    elif previous_sign == "My" and detected_sign == "Name":
                        play_sound("sounds/is.wav")
                        delayed_play(sound_file, delay=0.3)
                    # Normal sign
                    elif sound_file:
                        play_sound(sound_file)

                    previous_sign = last_detected_sign
                    last_detected_sign = detected_sign

                    asyncio.create_task(client.write_gatt_char(CHARACTERISTIC_UUID, detected_sign.encode(), response=False))
            else:
                candidate_sign = None
                candidate_count = 0

            root.after(0, update_gui, ax, ay, az, fingers, heartbeat, last_detected_sign)

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        try:
            while True:
                await asyncio.sleep(1)
        except asyncio.CancelledError:
            pass

        await client.stop_notify(CHARACTERISTIC_UUID)
        root.after(0, update_gui, 0, 0, 0, {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0}, "--", last_detected_sign, "Disconnected")

# Start BLE and GUI
threading.Thread(target=run_ble_loop, daemon=True).start()
root.mainloop()
