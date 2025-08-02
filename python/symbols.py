# symbols.py
import os

sign_definitions = [
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "1",
        "audio": "python/one.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "2",
        "audio": "python/two.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 0, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/three.wav"
    }
]
