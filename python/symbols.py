# symbols.py
import os
import time


sign_definitions = [
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "1",
        "audio": "sounds/one.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "2",
        "audio": "sounds/two.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 0, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "3",
        "audio": "sounds/three.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "4",
        "audio": "sounds/four.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "6",
        "audio": "sounds/six.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "7",
        "audio": "sounds/seven.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "8",
        "audio": "sounds/eight.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "9",
        "audio": "sounds/nine.wav"
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 0, "Ring": 0, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax < 200 and ay <-300,
        "sign": "Hello",
        "audio": "sounds/hello.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: 200 < ax < 600 and 700 < ay <1000,
        "sign": "My",
        "audio": "sounds/my.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 0, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: 400 < ax < 600 and 500 < ay <700,
        "sign": "Name",
        "audio": "sounds/name.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax < 0 and ay <100,
        "sign": "This",
        "audio": "sounds/this.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: -100 < ax < 200 and 0< ay <400,
        "sign": "Implementation",
        "audio": "sounds/implementation.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 1, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "None",
        "audio": ""
    },
    {
        "fingers": {"Thumb": 0, "Index": 0, "Middle": 1, "Ring": 1, "Little": 0},
        "conditions": lambda ax, ay, az, heartbeat: ax > 800,
        "sign": "Love",
        "audio": "sounds/love.wav"
    },
    {
        "fingers": {"Thumb": 1, "Index": 0, "Middle": 1, "Ring": 1, "Little": 1},
        "conditions": lambda ax, ay, az, heartbeat: -100 < ax < 100 and 800 < ay <1100,
        "sign": "I",
        "audio": "sounds/I.wav"
    }
]
