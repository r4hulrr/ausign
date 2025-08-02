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
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/four.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/five.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/six.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/seven.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/eight.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "python/nine.wav"
    }
]
