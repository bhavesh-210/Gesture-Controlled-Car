# Gesture-Controlled Car 🚗

An ESP32-based robotic car controlled through gesture commands, using ESP-NOW wireless communication, PWM motor control, and ultrasonic obstacle detection.

**Status:** Completed project

## Demo

Watch the car in action:

[Open the project demo video](docs/VIDEO-2026-10-23-02-05-10.mp4)

> GitHub can host MP4 files in a repository, but a YouTube link or a compressed GIF is usually easier to view and keeps the repository smaller. The current demo file is about 14.5 MB.

## Project Photos

![Gesture-controlled car — project photo 1](docs/PHOTO-2026-10-23-01-56-13.jpg)

![Gesture-controlled car — project photo 2](docs/PHOTO-2026-10-23-02-08-37.jpg)

## Circuit Diagram

The circuit image is currently stored as HEIC. If it doesn't render in the GitHub README, convert it to PNG or JPG and save it as `docs/circuit-diagram.png`.

![Circuit diagram](docs/circuit-diagram.png)

[Open the original circuit image](docs/circuit.heic)

## Features

- Wireless commands using ESP-NOW
- Forward, reverse, left, and right movement
- Braking control
- PWM-based motor speed control
- HC-SR04 ultrasonic obstacle detection
- Automatic braking and turning when an obstacle is detected

## Hardware

- ESP32 development boards
- Motor driver module
- DC motors and chassis
- HC-SR04 ultrasonic sensor
- Battery pack
- Gesture controller hardware

Confirm the exact controller sensor and motor-driver model from your final build before adding more specific component claims.

## Software

- Arduino IDE
- C++ with the Arduino framework
- ESP-NOW wireless communication
- PWM motor control

## Repository Structure

```text
Gesture-Controlled-Car/
├── GestureController/
│   └── GestureController.ino
├── GestureCarReceiver/
│   └── GestureCarReceiver.ino
├── 3D Cad/
│   └── *.stl
├── docs/
│   ├── PHOTO-2026-10-23-01-56-13.jpg
│   ├── PHOTO-2026-10-23-02-08-37.jpg
│   ├── VIDEO-2026-10-23-02-05-10.mp4
│   └── circuit.heic
├── hardware_headers/
└── README.md
```

## Pin Connections

The following pin assignments are documented in this project. Verify them against the final hardware before wiring.

| Component | ESP32 GPIO |
|---|---:|
| Left motor IN1 | 26 |
| Left motor IN2 | 25 |
| Right motor IN1 | 17 |
| Right motor IN2 | 16 |
| PWMA | 32 |
| PWMB | 33 |
| Ultrasonic TRIG | 18 |
| Ultrasonic ECHO | 19 |

**Safety:** If the HC-SR04 ECHO pin outputs 5 V, use an appropriate voltage divider or level shifter before connecting it to an ESP32 GPIO. ESP32 GPIOs are not 5 V tolerant.

## How It Works

1. The gesture controller interprets the user's movement input.
2. The controller sends movement commands to the car using ESP-NOW.
3. The receiver interprets the command and controls the motor driver.
4. The motor driver drives the DC motors in the requested direction and speed.
5. The ultrasonic sensor measures the distance to nearby objects.
6. The receiver's obstacle-response logic brakes or turns when its programmed conditions are met.

## Getting Started

### Requirements

- Arduino IDE with ESP32 board support installed
- The controller and receiver ESP32 boards used by this build
- Correctly wired motor driver, motors, sensor, and power supply

### Upload the Controller

1. Open `GestureController/GestureController.ino` in Arduino IDE.
2. Select the correct ESP32 board and serial port.
3. Check any board-specific settings and peer configuration required by the sketch.
4. Upload the firmware to the controller ESP32.

### Upload the Receiver

1. Open `GestureCarReceiver/GestureCarReceiver.ino`.
2. Select the receiver ESP32 board and serial port.
3. Configure any required ESP-NOW peer/MAC settings and libraries according to the sketch.
4. Upload the firmware to the receiver ESP32.

### Test the Car

1. Place the car on a clear, level surface and keep the wheels clear of obstructions.
2. Power the controller and receiver.
3. Test forward, reverse, left, right, and brake commands at low speed.
4. Test obstacle detection carefully and verify the car's response before operating near people or fragile objects.

Use the actual source code to confirm exact board settings, required libraries, wireless configuration, and startup behavior.

## Command Reference

| Command ID | Action |
|---:|---|
| 0 | Forward |
| 1 | Left |
| 2 | Right |
| 3 | Brake |
| 4 | Reverse toggle |

## 3D CAD Files

The `3D Cad/` directory contains STL files for the project's mechanical design. Open them in compatible CAD or slicer software to inspect or prepare them for fabrication.

## Testing

Add your measured or observed results so other people can reproduce your validation.

| Test | Result |
|---|---|
| ESP-NOW communication | Add observed result |
| Forward and reverse | Add observed result |
| Left and right turning | Add observed result |
| PWM speed control | Add observed result |
| Brake command | Add observed result |
| Ultrasonic obstacle detection | Add observed result |
| Automatic obstacle response | Add observed result |

## Challenges and Learnings

Document the actual engineering challenges you faced, the debugging steps you used, and how you solved them. Potential areas include ESP-NOW configuration, motor driver wiring, PWM tuning, sensor readings, power distribution, and mechanical assembly.

## Future Improvements

Possible extensions—not claims about current functionality—include:

- Adjustable obstacle-detection thresholds
- Battery-level monitoring
- Better gesture filtering
- Telemetry and diagnostic logging
- Cleaner wiring and a refined chassis

## Contributing

Suggestions and improvements are welcome. Fork the repository, create a feature branch, and open a pull request with a clear description of the change.

## License

No license is specified in this README. Add a `LICENSE` file if you want to define how others may use, modify, and distribute the project.

---

Built as a hands-on robotics project exploring embedded systems, wireless communication, motor control, and obstacle detection.
