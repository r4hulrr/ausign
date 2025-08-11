# symbols.py
import os
import time

# symbols.py
import csv

def load_sign_definitions(csv_path="python/sign_definitions.csv"):
    sign_definitions = []
    with open(csv_path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            sign_definitions.append({
                "fingers": {
                    "Thumb": int(row["Thumb"]),
                    "Index": int(row["Index"]),
                    "Middle": int(row["Middle"]),
                    "Ring": int(row["Ring"]),
                    "Little": int(row["Little"]),
                },
                "conditions": lambda ax, ay, az, heartbeat,
                              row=row: (
                    int(row["ax_min"]) <= ax <= int(row["ax_max"]) and
                    int(row["ay_min"]) <= ay <= int(row["ay_max"]) and
                    int(row["az_min"]) <= az <= int(row["az_max"]) and
                    int(row["heartbeat_min"]) <= heartbeat <= int(row["heartbeat_max"])
                ),
                "sign": row["sign"],
                "audio": row["audio"]
            })
    return sign_definitions

sign_definitions = load_sign_definitions()

