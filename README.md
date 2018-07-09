# YSWearables
## Panic Attack Detection Device

### Overview
This project is meant to read read temperature, sweat, heart rate, and motion data from a wearable armband to determine when the wearer is experiencing a panic attack.

The sensors used include:
- Accelerometer
- Pulse sensor
- Temperature and humidity sensor

The device will alert the user with:
- pulsing RGB LED
- vibrating module
- phone app (connected via bluetooth)

### Current Status
Currently the device can:
- read accelerometer individual raw values in x, y, and z directions
- read BPM from pulse sensor and create real-time graph within Arduino serial plotter
- read temperature and humidity

Note: none of these functionalities have been tested yet.

### ToDo
- test all sensor functionality
- send all info to phone app via bluetooth (ongoing)
- create algorithm to determine excessive motion from accelerometer
- make sure pulse sensor doesn't interrupt other readings
- ensure accuracy of humidity sensor, decide how to use readings to measure sweat
- create breathing exercise with LED
- make vibe board vibrate when BPM is too high and/or motion is erratic