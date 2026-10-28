---
title: "Gesture-Controlled Car — The Nonchalant"
github: "bhavesh-210/Gesture-Controlled-Car"
description: "A 4WD robot car controlled using hand gestures via ESP32 and ESP-NOW, featuring PWM speed control, ultrasonic collision avoidance, and a custom 3D printed chassis."
total_time_spent: "21h"
---

# Gesture-Controlled Car ("The Nonchalant") — Build Journal

**Total project time logged:** 21 hours (across 5 development entries)

For this project, I built a 4WD gesture-controlled robot car called "The Nonchalant". Instead of using a smartphone app, Bluetooth game controller, or bulky RC transmitter, the car responds directly to natural hand movements.

The setup uses two ESP32 development boards communicating over ESP-NOW:
1. A wearable hand controller fitted with an MPU-9250 IMU that reads tilt angles (pitch and roll).
2. An onboard car receiver that translates received commands into motor signals via an L298N dual H-bridge motor driver with PWM speed regulation.
3. An HC-SR04 ultrasonic distance sensor on the front bumper that acts as an emergency stop and collision avoidance guard if an obstacle gets too close.
4. A custom 3D-printed chassis designed in Fusion 360 to house the motors, boards, and battery pack securely.

---

## 1. Project Overview & Concept

![The Nonchalant - Completed Gesture Controlled Car Chassis](docs/PHOTO-2026-10-23-01-56-13.jpg)
*Figure 1: Completed car featuring custom 3D-printed chassis, four TT geared DC motors, and front bumper plate.*

**Time spent:** 2h 30m

### Why I Built This
I wanted to build an intuitive, wearable remote-controlled vehicle that feels natural to drive. Most DIY cars use Bluetooth or Wi-Fi apps on a phone, which require looking down at a screen and often suffer from latency or connection drops.

Using ESP-NOW between two ESP32 microcontrollers gives direct, peer-to-peer wireless communication with virtually zero noticeable latency (under 5 ms) and without needing a local Wi-Fi router.

### Core Features
- Direct peer-to-peer wireless: ESP-NOW 2.4 GHz protocol connects the transmitter directly to the car receiver.
- Natural tilt controls: 5 motion states (Forward, Left turn, Right turn, Brake/Stop, and Reverse toggle) calculated from accelerometer pitch and roll.
- Hardware PWM speed control: ESP32 hardware LEDC channels regulate motor speed at 1000 Hz for smooth acceleration.
- Automatic collision avoidance: HC-SR04 sensor detects obstacles within 20 cm and automatically brakes and steers away.
- Custom 3D printed chassis: Custom plates and motor mounts designed in Fusion 360 to fit four TT yellow gearmotors and all onboard electronics.

### Video Demonstration
I recorded an operational test showing the hand controller steering the car and the car driving:
- [Watch or download the demo video](docs/VIDEO-2026-10-23-02-05-10.mp4)

---

## 2. Hardware Selection, Circuit Wiring & Bill of Materials

![Gesture-Controlled Car Circuit Diagram](docs/circuit-diagram.png)
*Figure 2: Wiring schematic for the hand transmitter ESP32 and MPU-9250 sensor (source preserved at docs/circuit.heic).*

![Chassis Internal Electronics and Wiring](docs/PHOTO-2026-10-23-02-08-37.jpg)
*Figure 3: Inside the car chassis: ESP32 receiver, L298N motor driver, TT motors, battery pack, and power switch.*

**Time spent:** 5 hours

### Bill of Materials (BOM)

| Item | Component | Qty | Unit Price (INR) | Total (INR) | Buying Source | Role in Build |
| :---: | :--- | :---: | :---: | :---: | :--- | :--- |
| **1** | ESP32 Development Board (ESP-WROOM-32) | 2 | ₹400.00 | ₹800.00 | [ElectronicsComp](https://www.electronicscomp.com/esp32-development-board-with-wifi-bluetooth-india) | 1x hand transmitter, 1x car receiver |
| **2** | MPU-9250 9-Axis IMU Module | 1 | ₹442.00 | ₹442.00 | [ElectronicsComp](https://www.electronicscomp.com/development-board-programmer/arduino/mpu9250-9-axis-gyro-accelerometer-module?limit=50) | Tilt angle sensing (pitch/roll) on controller |
| **3** | L298N Dual H-Bridge Motor Driver | 1 | ₹186.00 | ₹186.00 | [ElectronicsComp](https://www.electronicscomp.com/stepper-motor-drivers/l298n-dual-h-bridge-dc-stepper-motor-driver-controller-module) | Drives the 4 DC motors with direction & PWM speed |
| **4** | TT Geared DC Motors with Rubber Wheels | 4 | ₹128.00 | ₹512.00 | [Robu.in](https://robu.in/product-category/straight-bo-motors/) | 4WD drivetrain (paired left and right) |
| **5** | 65 mm Wheels for BO Motors | 4 | ₹45.00 | ₹180.00 | [ElectronicsComp](https://www.electronicscomp.com/65mm-robot-wheel-for-bo-motors-yellow) | Wheels pressed onto motor shafts |
| **6** | HC-SR04 Ultrasonic Distance Sensor | 1 | ₹79.00 | ₹79.00 | [Robu.in](https://robu.in/product/hc-sr04-ultrasonic-range-finder/) | Front bumper obstacle clearance detection |
| **7** | 7.4V 2S Li-ion Rechargeable Battery Pack | 1 | ₹765.00 | ₹765.00 | [ElectronicsComp](https://www.electronicscomp.com/7.4v-2600mah-li-ion-battery-pack-inbuilt-bms-protection) | Main power supply for car and motors |
| **8** | Jumper Wires (M-F / M-M pack) | 1 pack | ₹64.00 | ₹64.00 | [ElectronicsComp](https://www.electronicscomp.com/male-to-female-jumper-wires-10cm-40-pieces-pack) | Circuit connections and prototyping |
| **9** | SPST Rocker Switch | 1 | ₹20.00 | ₹20.00 | Local shop | Main battery disconnect switch |
| **10** | Custom 3D-Printed Chassis Parts | 1 set | ₹0.00 | ₹0.00 | Self-printed ([3D Cad/](3D%20Cad)) | Baseplate, deck plate, bumper, brackets |

### Hand Controller Wiring (I2C)
The hand transmitter connects the ESP32 to the MPU-9250 sensor over I2C:

| ESP32 Pin | MPU-9250 Pin | Wire Color (in diagram) | Function |
| :--- | :--- | :--- | :--- |
| **3V3** | VCC | Red | 3.3V Power |
| **GND** | GND | Black | Ground |
| **GPIO 14** | SCL | Green | I2C Clock (`Wire.begin(15, 14)`) |
| **GPIO 15** | SDA | Blue | I2C Data (`Wire.begin(15, 14)`) |

### Car Receiver Pin Mapping
The onboard ESP32 connects to the L298N motor driver and HC-SR04 ultrasonic sensor:

| Signal | ESP32 GPIO | Direction | Connected To / Function |
| :--- | :--- | :--- | :--- |
| **LIN1** | GPIO 26 | Output | L298N IN1 (Left motor direction 1) |
| **LIN2** | GPIO 25 | Output | L298N IN2 (Left motor direction 2) |
| **RIN1** | GPIO 17 | Output | L298N IN3 (Right motor direction 1) |
| **RIN2** | GPIO 16 | Output | L298N IN4 (Right motor direction 2) |
| **PWMA** | GPIO 32 | Output (PWM) | L298N ENA (Left motor speed control) |
| **PWMB** | GPIO 33 | Output (PWM) | L298N ENB (Right motor speed control) |
| **TRIG_PIN** | GPIO 18 | Output | HC-SR04 Trigger pin (10 µs pulse) |
| **ECHO_PIN** | GPIO 19 | Input | HC-SR04 Echo pin (pulse duration) |

*Note on ECHO pin voltage:* The HC-SR04 is powered from 5V and outputs a 5V echo signal. Since the ESP32 GPIO pins are rated for 3.3V, I put a simple resistor divider (1kΩ and 2kΩ) on the echo line to safely step the voltage down to ~3.3V before feeding it into GPIO 19.

---

## 3. System Architecture & Firmware Implementation

![Gesture Transmitter Wiring and Architecture](docs/circuit-diagram.png)
*Figure 4: Transmitter schematic showing I2C communication between ESP32 and MPU-9250.*

**Time spent:** 6h 30m

### System Flow
The overall control pipeline works like this:
1. The MPU-9250 on the transmitter reads acceleration values and passes them to the ESP32 via I2C.
2. The controller calculates pitch and roll angles and compares them against a 15-degree threshold.
3. Every 100 ms, the controller transmits the resulting command packet over ESP-NOW to the car's MAC address.
4. The receiver ESP32 catches the packet in an interrupt callback and drives the L298N motor pins and PWM channels accordingly.
5. In parallel, the receiver checks the HC-SR04 ultrasonic sensor. If an obstacle is detected within 20 cm, it overrides the command, applies the brake, and pivots away.

### How the Hand Controller Detects Gestures
In the transmitter code ([GestureController/GestureController.ino](GestureController/GestureController.ino)), the MPU-9250 measures acceleration along the X, Y, and Z axes. From those values, I compute the tilt angles (pitch and roll) in degrees:

```cpp
float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI;
float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
```

Once pitch and roll are calculated, the `detectCommand(pitch, roll)` function checks if the tilt exceeds a 15-degree threshold:

| Command ID | Firmware Name | Hand Gesture Trigger | Car Movement |
| :---: | :--- | :--- | :--- |
| **0** | `FORWARD` | Hand tilted forward (`pitch > 15°`) | Both left and right motors run forward at set speed (180/255). |
| **1** | `LEFT_CMD` | Hand tilted left (`roll < -15°`) | Left motor stops, right motor runs forward (skid-steer left). |
| **2** | `RIGHT_CMD` | Hand tilted right (`roll > 15°`) | Right motor stops, left motor runs forward (skid-steer right). |
| **3** | `BRAKE_CMD` | Hand kept level (`-15° <= pitch, roll <= 15°`) | All motor pins pulled LOW; PWM set to 0 (car stops). |
| **4** | `REVERSE_CMD` | Hand tilted backwards (`pitch < -15°`) | Toggles the reverse direction mode so the car backs up. |

### ESP-NOW Wireless Communication
I used ESP-NOW for wireless communication because it is lightweight and transmits directly without router overhead. I created a simple struct for sending the active command:

```cpp
typedef struct struct_message {
  int comand_id;
} struct_message;
```

The transmitter pairs with the car's MAC address (`esp_now_add_peer`) and sends this packet every 100 ms in the main loop.

On the car side, an incoming packet triggers the `OnDataRecv` callback function, which unpacks the command ID and calls the matching motor function (`Forward()`, `Left()`, `Right()`, `Brake()`, or `Reverse()`).

### Motor Control & Obstacle Avoidance Logic
In [GestureCarReceiver/GestureCarReceiver.ino](GestureCarReceiver/GestureCarReceiver.ino):

1. **PWM Speed Control:** I configured two hardware PWM channels (`CH_A` and `CH_B`) using the ESP32 LEDC peripheral at 1000 Hz with 8-bit resolution (0 to 255). I set `motorSpeed = 180` (~70% duty cycle), which provides good driving speed and torque without overheating the L298N driver.
2. **Reverse Direction Toggle:** When `Reverse()` is called, it toggles a `Reversed` boolean flag. The `CheckReverse()` helper updates the direction pins `Out1` and `Out2`:
   - Normal mode: `Out1 = HIGH`, `Out2 = LOW`
   - Reverse mode: `Out1 = LOW`, `Out2 = HIGH`
3. **Ultrasonic Distance Measurement:** The `getDistance()` function sends a 10 µs trigger pulse to the HC-SR04 on GPIO 18 and reads the echo return time on GPIO 19:
   ```cpp
   long duration = pulseIn(ECHO_PIN, HIGH, 30000);
   if (duration == 0) return 999;
   return duration * 0.034 / 2;
   ```
   If the distance is 20 cm or less:
   - In `Forward()`: The car immediately aborts driving and applies the brakes.
   - In `loop()`: If an obstacle is detected, the car brakes, pauses 200 ms, turns right for 350 ms, and brakes again to clear the path.

### Uploading the Firmware
1. Open the sketches in the Arduino IDE (make sure ESP32 board support is installed).
2. Install the `MPU9250_asukiaaa` library from the Arduino Library Manager.
3. Find the MAC address of the receiver ESP32 by running `WiFi.macAddress()` in a temporary sketch.
4. Update the `carMAC` array in `GestureController/GestureController.ino` with the car's MAC address.
5. Connect each board via USB and flash `GestureController.ino` to the controller ESP32 and `GestureCarReceiver.ino` to the car ESP32.

---

## 4. Testing, Debugging & Lessons Learned

![Benchtop Electronics Assembly and Testing](docs/PHOTO-2026-10-23-02-08-37.jpg)
*Figure 5: Testing the electronics and motor driver on the workbench.*

**Time spent:** 4 hours

### Testing Stages & Results
- **Benchtop Motor Test:** Before mounting the wheels to the ground, I set the car on a stand with the wheels elevated. I tilted the hand controller in each direction and confirmed that the left and right wheels turned forward, reversed, and stopped as expected.
- **Wireless Response:** ESP-NOW responded instantly. The car reacted to hand tilts with no perceptible delay across the room.
- **Obstacle Sensor Validation:** I held a flat cardboard piece in front of the car while it was moving forward. As soon as it crossed within 20 cm, the car stopped and turned right to clear the obstacle.
- **Driving Demonstration:** I assembled the chassis and took the car for an operational test on the floor to verify turning, stopping, and general handling. The video is saved in [docs/VIDEO-2026-10-23-02-05-10.mp4](docs/VIDEO-2026-10-23-02-05-10.mp4).

### Key Challenges & Fixes
1. **L298N Voltage Drop:** The L298N driver is reliable and cheap, but its bipolar internal transistors cause a voltage drop of around 1.5V to 2.0V. When testing with 4x AA batteries (around 4.8V to 5V), the motors struggled with torque. Upgrading to a 7.4V 2S Li-ion battery provided sufficient voltage for the motors to run cleanly under the weight of the chassis.
2. **Reverse Gesture Repetition:** In the transmitter code, tilting backward sends command `4` every 100 ms. Because the receiver toggles `Reversed = !Reversed` on each packet, holding the tilt back caused the reverse flag to toggle rapidly on and off. During testing, I learned to tilt back briefly to switch directions. For the next firmware update, I plan to add a debounce / state check on the transmitter so it only sends a single toggle packet per gesture.
3. **Blocking `pulseIn()` Timeout:** If the ultrasonic sensor doesn't receive an echo, `pulseIn()` can block for up to 30 ms. While 30 ms is relatively short, running the distance measurement via a non-blocking timer or FreeRTOS task would keep the main control loop running even smoother.

---

## 5. 3D Chassis Design, Future Improvements & Project Info

![Completed Car Chassis Assembly](docs/PHOTO-2026-10-23-01-56-13.jpg)
*Figure 6: Custom 3D-printed chassis assembled with four TT motors and electronics.*

**Time spent:** 3 hours

### 3D CAD Modeling
I designed the chassis specifically for this project in Autodesk Fusion 360. All source files and 3D printable models are located in the [3D Cad/](3D%20Cad) folder:

- **Autodesk Fusion 360 Source:**
  - `The Nonchalant (1).f3d`: Full design project file with editable sketches, parameters, and timeline.
- **STEP Solid Models (Ready for 3D Printing / Slicing):**
  - `The Nonchalant.step`: Front bumper plate with the embossed car name
  - `Body22.step`: Lower chassis baseplate for mounting the motors and battery
  - `Body51.step`: Upper deck plate holding the ESP32 and motor driver
  - `Component2.step`: Motor mounting standoffs
  - `Body25.step`: Structural frame brackets
  - `Body21 (...).step`: Side wheel clearance plates

All parts were printed in standard PLA filament on an FDM 3D printer. The STEP files can be imported into any slicer (PrusaSlicer, Cura, Bambu Studio) or opened in CAD software like Fusion 360 or FreeCAD for customization.

### Future Improvements
- Upgrade to a MOSFET motor driver (such as the TB6612FNG or DRV8833) to reduce voltage drop, generate less heat, and extend battery runtime.
- Add 2-way telemetry feedback over ESP-NOW so the car can send battery voltage warnings or obstacle alerts back to the controller.
- Implement proportional steering by mapping the roll angle directly to motor PWM values for smoother, gradual turns instead of fixed-speed pivot turns.
- Make the ultrasonic distance check non-blocking using FreeRTOS tasks or hardware timers.

### Repository Files
```text
Gesture-Controlled-Car/
├── 3D Cad/                                # Fusion 360 source & STEP models
│   ├── The Nonchalant (1).f3d             # Parametric CAD file
│   ├── The Nonchalant.step                # Front nameplate model
│   ├── Body22.step                        # Lower chassis baseplate
│   ├── Body51.step                        # Upper deck plate
│   ├── Component2.step                    # Motor mounting standoff
│   ├── Body25.step                        # Frame bracket
│   └── Body21 (...).step                  # Side clearance panel
├── GestureCarReceiver/
│   └── GestureCarReceiver.ino             # Car receiver, motor control & obstacle code
├── GestureController/
│   └── GestureController.ino             # Hand controller & gesture detection code
├── docs/
│   ├── PHOTO-2026-10-23-01-56-13.jpg     # Car exterior photo
│   ├── PHOTO-2026-10-23-02-08-37.jpg     # Internal wiring photo
│   ├── VIDEO-2026-10-23-02-05-10.mp4     # Driving test video
│   ├── circuit-diagram.png               # Circuit schematic
│   └── car-3d-model.svg                  # Chassis wireframe diagram
├── hardware_headers/                      # Header stubs for code editor linting
├── BOM.csv                                # Bill of materials with parts and pricing
├── JOURNAL.md                             # 5-stage project build journal
└── README.md                              # Main project documentation
```

### Author
Project built and documented by Bhavesh Sharma ([@bhavesh-210](https://github.com/bhavesh-210)). Feel free to use the CAD files and code to build your own gesture-controlled car!
