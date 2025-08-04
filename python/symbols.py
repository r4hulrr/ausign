# symbols.py
import os
import time


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
        "sign": "4",
        "audio": "python/four.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "5",
        "audio": "python/five.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "6",
        "audio": "python/six.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "7",
        "audio": "python/seven.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "8",
        "audio": "python/eight.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "9",
        "audio": "python/nine.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax < 200 and ay <-300,
        "sign": "Wave left",
        "audio": ""
    },
    {
        "fingers": {"Thumb": 1, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: 200 < ax < 600 and 700 < ay <1000,
        "sign": "My",
        "audio": ""
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: 400 < ax < 600 and 500 < ay <700,
        "sign": "Name",
        "audio": ""
    }
]
