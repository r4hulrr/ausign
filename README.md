# AUSIGN – Real-Time Auslan Sign Language Interpreter

 **Breaking communication barriers** for the Australian Deaf community with an affordable, wearable, real-time Auslan-to-speech translation glove. Check out the [demo video](demo/demo_video.mov).


## Overview
**AUSIGN** is a wearable assistive technology device designed to translate **Australian Sign Language (Auslan)** into spoken English **in real time**, enabling smoother and more natural communication for deaf and non-verbal individuals.

Unlike existing ASL-centric devices, AUSIGN is **purpose-built for Auslan**, incorporating contextual awareness via a heart-rate sensor, low-latency audio feedback, and Bluetooth connectivity for flexible output through a mobile/desktop app.


##  Key Features
- **Auslan-specific design** – recognises grammar and gestures unique to Auslan.
- **Context-aware translation** – heartbeat sensing to infer emotional tone.
- **Low-latency output** – spoken translation in <1 second for natural conversations.
- **Comfort & portability** – under 150 g, designed for all-day wear.
- **Custom sign mapping** – add personal or region-specific gestures via app.
- **Cross-platform UI** – BLE-connected Python GUI (future iOS/Android support).
- **Rechargeable & all-day battery life** – ~6 hours continuous operation.


## System Architecture

**Hardware**
- **Sensors:**  
  - 5× flex sensors for finger bend detection  
  - LSM6DSOTR 6-axis IMU for hand motion/orientation  
  - MAX30105 heartbeat sensor for emotional context  
- **Processing:** ESP32-S3 microcontroller with BLE & I²S audio output  
- **Output:** OLED display + speaker (via MAX98357A DAC/amp), 2× customisable push buttons  
- **Power:** 3.7 V Li-Po battery with BQ24074 power management & charging

**Software**
- **Firmware:** Reads sensor data, sends via BLE to host app, plays audio from internal flash.  
- **Recognition algorithm:** Condition-matching engine using flex, IMU, and BPM thresholds from CSV definitions.  
- **GUI:** Python-based BLE interface for displaying recognised signs, sensor status, and audio playback.  

---

## Repository Structure
```
├── firmware/ # ESP32-S3 C++ firmware (Arduino/ESP-IDF)
│ ├── src # Contains the main esp32 code along with test code for every component
│ ├── partitions # Custom partition implemented for our 16MB Flash size
├── gui/ # Python BLE GUI application
│ ├── frontend.py # main GUI script
│ ├── signs.csv # Sign definition mappings
│ └── ...
├── hardware/ # PCB schematics and board layouts
│ ├── main_board/
│ ├── breakout_board/
├── demo # Demo video
│ ├── demo_video.mp4
├── docs/ # design docs
└── LICENSE
└── README.md
```


## Demo
Check out the [demo video](demo/demo_video.mov) to see AUSIGN in action.


## License
This project is licensed under the [MIT License](LICENSE).


## Authors
- **Rahul Ramachandran** 
- **Enzo Moriguchi** 

---
> *“Technology is best when it brings people together.”* – AUSIGN aims to bridge the communication gap and foster inclusion through engineering innovation.
