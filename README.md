# Gesture-Controlled Car — The Nonchalant

A small four-wheel-drive robot that I can steer by tilting my hand instead of using a phone or a conventional remote.

![The assembled car](docs/PHOTO-2026-10-23-01-56-13.jpg)

The idea is straightforward: an MPU-9250 motion sensor reads the tilt of a handheld controller, an ESP32 turns that movement into a command, and a second ESP32 on the car receives it over ESP-NOW. An L298N motor driver handles the wheels. I also added an ultrasonic sensor at the front so the car can react when something is close to it.

This repository contains both Arduino sketches, the wiring reference, the component list, the Fusion 360 design, and photos and a short video of the build. The more personal notes about the process are in [JOURNAL.md](JOURNAL.md).

## See it in action

- [Driving demo](docs/VIDEO-2026-10-23-02-05-10.mp4)
- [Photo of the assembled car](docs/PHOTO-2026-10-23-01-56-13.jpg)
- [Inside the chassis](docs/PHOTO-2026-10-23-02-08-37.jpg)

## What it does

I wanted the controls to feel simple enough that I wouldn't have to think about buttons. The controller checks its pitch and roll and chooses one of five commands:

| Hand movement | Command | What the car does |
| --- | --- | --- |
| Tilt forward | `FORWARD` | Drives both sides forward |
| Tilt left | `LEFT_CMD` | Stops the left wheels and runs the right wheels |
| Tilt right | `RIGHT_CMD` | Stops the right wheels and runs the left wheels |
| Hold level | `BRAKE_CMD` | Stops the motors |
| Tilt backward | `REVERSE_CMD` | Toggles the reverse-direction setting |

The threshold is **15°** in the current sketch. Pitch takes priority over roll, so a strong forward or backward tilt wins if the hand is tilted diagonally. The controller sends a command roughly every **100 ms**.

Reverse is worth explaining: the backward gesture does **not** continuously issue a reverse-drive command. It toggles a direction flag on the receiver. The next drive command uses that direction. As the code stands, keeping the hand tilted backward can flip the flag repeatedly; adding gesture debouncing is on my list of fixes.

## How the pieces work together

```text
Hand controller                          Car
MPU-9250                                ESP32 receiver
   | I2C                                   | 
ESP32 transmitter -- ESP-NOW packet -->   +-- L298N --> 4 DC motors
                                          |
                                          +-- HC-SR04 distance sensor
```

Only a small command ID travels over the wireless connection; the car decides how to drive its motors based on that ID. ESP-NOW works directly between the two ESP32 boards, so the project doesn't need a Wi-Fi router or a phone app.

The receiver uses two **1 kHz, 8-bit PWM** channels, one for the left motors and one for the right. The current speed setting is `180` out of `255`. This is a fixed duty cycle, not proportional speed control: tilting your hand farther does not yet make the car faster.

### Obstacle response

The HC-SR04 checks the distance in front of the car. When it reads **20 cm or less**, the receiver's main loop brakes, pauses for **200 ms**, turns right for **350 ms**, and brakes again. The `Forward()` function also checks distance before driving. This is a basic obstacle response, not full navigation or obstacle mapping, and it may not avoid every collision. The ultrasonic reading and the timed turn can block parts of the program; there is room to improve that.

## Parts used

| Part | Quantity | Purpose |
| --- | ---: | --- |
| ESP32 development board (ESP-WROOM-32) | 2 | One for the hand controller, one for the car |
| MPU-9250 IMU | 1 | Measures hand movement |
| L298N dual H-bridge driver | 1 | Runs the left and right motor pairs |
| 6V TT/BO geared motors | 4 | Four-wheel drivetrain |
| 65 mm wheels | 4 | Wheels for the geared motors |
| HC-SR04 ultrasonic sensor | 1 | Measures distance to objects ahead |
| 7.4V 2S rechargeable battery pack | 1 | Car power source |
| Jumper wires and rocker switch | As needed | Connections and power control |
| Custom printed chassis parts | 1 set | Holds the hardware together |

See [BOM.csv](BOM.csv) for the itemized parts, example supplier links, and approximate prices. The listed prices are estimates, not a guarantee of current costs. Printing costs are not included in the BOM's ₹0 chassis entry.

## Wiring

![Controller circuit diagram](docs/circuit-diagram.png)

### Controller: ESP32 to MPU-9250

| ESP32 | MPU-9250 |
| --- | --- |
| `3V3` | `VCC` |
| `GND` | `GND` |
| `GPIO 15` | `SDA` |
| `GPIO 14` | `SCL` |

The sketch uses `Wire.begin(15, 14)`.

### Car: ESP32 pins

| Function | ESP32 GPIO |
| --- | --- |
| L298N left direction 1 (`LIN1`) | 26 |
| L298N left direction 2 (`LIN2`) | 25 |
| L298N right direction 1 (`RIN1`) | 17 |
| L298N right direction 2 (`RIN2`) | 16 |
| Left speed / L298N `ENA` | 32 |
| Right speed / L298N `ENB` | 33 |
| HC-SR04 `TRIG` | 18 |
| HC-SR04 `ECHO` | 19 |

**Important:** A typical HC-SR04 powered at 5 V can output a 5 V echo pulse, while an ESP32 GPIO is **not 5 V tolerant**. Use a suitable level shifter or voltage divider on the echo signal before connecting it to GPIO 19. Also make sure the receiver, driver, and sensors have the correct supplies and a shared ground. Do not connect the 7.4 V battery directly to an ESP32's 3.3 V pin. Check the power path against your specific board and modules before switching the car on.

## Getting the code running

This is an Arduino-based ESP32 project. You'll need the Arduino IDE, an ESP32 board package, and the `MPU9250_asukiaaa` library for the controller sketch.

1. Clone the repository:

   ```bash
   git clone https://github.com/bhavesh-210/Gesture-Controlled-Car.git
   cd Gesture-Controlled-Car
   ```

2. Install **ESP32 by Espressif Systems** through Arduino IDE's Boards Manager and install **MPU9250_asukiaaa** through Library Manager.
3. Find the car ESP32's **station MAC address**, for example by running a short sketch that prints `WiFi.macAddress()` after `WiFi.mode(WIFI_STA)`.
4. Replace the example address in `GestureController/GestureController.ino` with the actual receiver address:

   ```cpp
   uint8_t carMAC[] = { 0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC }; // example only
   ```

5. Upload [GestureCarReceiver.ino](GestureCarReceiver/GestureCarReceiver.ino) to the car ESP32 and [GestureController.ino](GestureController/GestureController.ino) to the controller ESP32.
6. For initial checks, **keep the drive wheels off the ground**. Open Serial Monitor at **115200 baud** on the controller and check which commands appear as you tilt it. Verify the motor directions and neutral/stop behavior before trying it on a clear floor.

**ESP32 core compatibility:** The motor code uses `ledcSetup` and `ledcAttachPin`, which are from the Arduino-ESP32 2.x LEDC API. It also uses the newer `esp_now_recv_info` receive callback form. Depending on your installed core version, these APIs may need to be reconciled before the receiver sketch compiles. I have not included a confirmed working core version here, so treat a compile check as part of setup rather than assuming it will work unchanged on every ESP32 installation.

## The chassis

![CAD view of the car](docs/car-3d-model.svg)

The chassis is not an off-the-shelf car frame. I designed the layout in Fusion 360 to leave space for four motors, electronics, and the battery. The [3D Cad folder](3D%20Cad/) has the editable `.f3d` archive and `.step` files for the individual parts and assembly. The STEP files are solid CAD models; depending on your slicer, you may need to import them into CAD software and export printable meshes first.

![Hardware inside the car](docs/PHOTO-2026-10-23-02-08-37.jpg)

## Things I learned (and things I'd change)

The biggest takeaway was that the car is more than just two sketches talking to each other. Motor voltage, the physical layout, wireless behavior, and the timing of sensor reads all matter once the wheels touch the ground.

A few limitations are still in the current code:

- **Reverse toggling:** one held gesture can trigger several toggles. It needs an edge-triggered command or debounce logic.
- **No wireless timeout stop:** the receiver doesn't currently stop the motors just because controller packets stop arriving. I'd add a watchdog before treating this as a dependable remote-controlled car.
- **Blocking distance measurement:** `pulseIn()` can wait up to 30 ms; the obstacle-turn routine also uses delays.
- **Fixed steering speed:** turns are on/off rather than scaled to hand tilt.
- **Hardware and ESP32 core variation:** pin voltage levels and library versions need careful checking for each build.

My next changes would be a communication timeout, more reliable reverse handling, and proportional steering. A more efficient driver such as the TB6612FNG could also reduce losses compared with the L298N, provided it is appropriately rated for the motors.

## Repository guide

```text
GestureController/      Hand-motion reading and ESP-NOW transmitter sketch
GestureCarReceiver/     ESP-NOW receiver, motor control and obstacle logic
3D Cad/                Fusion 360 project and STEP solid models
docs/                  Photos, circuit reference, CAD preview and demo video
BOM.csv                Component list and example pricing
JOURNAL.md             Development notes and reflections
README.md              Overview and setup instructions
```

## About this build

I built **The Nonchalant** as a practical robotics project to learn how sensing, wireless communication, electronics, mechanical design, and embedded code work together. It is a hobby-scale prototype, not a safety-certified vehicle.

**Bhavesh Sharma** · [GitHub @bhavesh-210](https://github.com/bhavesh-210)
