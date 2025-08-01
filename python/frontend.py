import asyncio
import struct
import os
import tkinter as tk
from bleak import BleakClient, BleakScanner
import pygame
import threading

# ble settings
DEVICE_NAME = "Auslan_glove"
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

# audio
audio_initialized = False
is_muted = False

def play_sound(filename):
    global audio_initialized, is_muted
    if is_muted:
        return
    try:
        if not audio_initialized:
            pygame.mixer.init()
            audio_initialized = True
        pygame.mixer.music.load(filename)
        pygame.mixer.music.play()
    except Exception as e:
        print(f"Error playing sound: {e}")

# gui
root = tk.Tk()
root.title("Auslan Glove Monitor")
root.geometry("400x300")
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
sign_label.pack(pady=20)

def toggle_mute():
    global is_muted
    is_muted = not is_muted
    mute_button.config(text="Unmute" if is_muted else "Mute")

mute_button = tk.Button(root, text="Mute", command=toggle_mute)
mute_button.pack(pady=5)

status_label = tk.Label(root, text="Status: Scanning...", font=("Helvetica", 10), fg="gray")
status_label.pack(pady=10)

# async ble task
def run_ble_loop():
    asyncio.run(main())

def update_gui(ax, ay, az, fingers, heartbeat, detected_sign):
    accel_label.config(text=f"Accel: ({ax}, {ay}, {az})")
    for finger, value in fingers.items():
        finger_labels[finger].config(text=f"{finger}: {value}")
    heartbeat_label.config(text=f"Heartbeat: {heartbeat}")
    sign_label.config(text=f"Detected Sign: {detected_sign if detected_sign else 'None'}")

async def main():
    status_label.config(text="Scanning for ESP32...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)

    if not device:
        status_label.config(text="Device not found.")
        return

    status_label.config(text=f"Found {DEVICE_NAME}, connecting...")

    async with BleakClient(device) as client:
        status_label.config(text="Connected. Listening for data...")

        last_detected_sign = None

        def notification_handler(sender, data):
            nonlocal last_detected_sign

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

            if (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 1 and
                fingers["Ring"] == 1 and
                fingers["Little"] == 1 and
                ax > 800
            ):
                detected_sign = "1"
                sound_file = os.path.abspath("python/one.wav")

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 1 and
                fingers["Little"] == 1 and
                ax > 800
            ):
                detected_sign = "2"
                sound_file = os.path.abspath("python/two.wav")

            elif (
                fingers["Thumb"] == 1 and
                fingers["Index"] == 0 and
                fingers["Middle"] == 0 and
                fingers["Ring"] == 0 and
                fingers["Little"] == 1 and
                ax > 800
            ):
                detected_sign = "3"
                sound_file = os.path.abspath("python/three.wav")

            if detected_sign and detected_sign != last_detected_sign:
                last_detected_sign = detected_sign
                if sound_file:
                    play_sound(sound_file)
                asyncio.create_task(client.write_gatt_char(CHARACTERISTIC_UUID, detected_sign.encode(), response=False))
            elif detected_sign is None:
                last_detected_sign = None

            # thread safe ui update
            root.after(0, update_gui, ax, ay, az, fingers, heartbeat, detected_sign)

        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        try:
            while True:
                await asyncio.sleep(1)
        except asyncio.CancelledError:
            pass

        await client.stop_notify(CHARACTERISTIC_UUID)
        status_label.config(text="Disconnected.")

# start ble
threading.Thread(target=run_ble_loop, daemon=True).start()
root.mainloop()
