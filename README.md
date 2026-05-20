# Gesture Controlled Car

ESP32-based gesture controlled car using ESP-NOW wireless communication, motor driver control, PWM speed control, and ultrasonic obstacle detection.

## Features

- ESP-NOW wireless control
- Forward, left, right, brake, and reverse modes
- PWM motor speed control
- Ultrasonic obstacle detection
- Automatic brake and turn when obstacle is detected

## Hardware Used

- ESP32
- Motor driver module
- DC motors
- Ultrasonic sensor HC-SR04
- Battery pack
- Gesture transmitter/controller

## Pin Connections

| Component       | ESP32 Pin |
| --------------- | --------- |
| Left Motor IN1  | 26        |
| Left Motor IN2  | 25        |
| Right Motor IN1 | 17        |
| Right Motor IN2 | 16        |
| PWMA            | 32        |
| PWMB            | 33        |
| Ultrasonic TRIG | 18        |
| Ultrasonic ECHO | 19        |

## Commands

| Command ID | Action         |
| ---------- | -------------- |
| 0          | Forward        |
| 1          | Left           |
| 2          | Right          |
| 3          | Brake          |
| 4          | Reverse Toggle |

## Code

Main code is inside:

```text
GestureCarReceiver/GestureCarReceiver.ino
```
