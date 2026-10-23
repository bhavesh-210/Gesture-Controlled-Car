# Gesture-Controlled Car — The Nonchalant 🚗

An ESP32-based robotics project that controls a four-wheel car using hand-tilt gestures, ESP-NOW wireless commands, PWM motor control, and HC-SR04 ultrasonic obstacle detection.

![Completed Gesture-Controlled Car](docs/PHOTO-2026-10-23-01-56-13.jpg)

**Project status:** Physical prototype and firmware implemented. See the testing section below for items that still require documented physical validation.

## 1. Project Overview

The controller reads movement from an MPU-9250 inertial measurement unit (IMU), classifies the hand tilt into a motion command, and sends that command to the car receiver using ESP-NOW. The receiver controls the L298N motor driver and checks the forward path with an HC-SR04 ultrasonic sensor.

### Demo

[Watch or download the project demo video](docs/VIDEO-2026-10-23-02-05-10.mp4)

### Main features

- ESP-NOW wireless command transfer between two ESP32 boards.
- Forward, left, right, brake, and reverse-toggle commands.
- PWM motor speed control using the ESP32 LEDC peripheral.
- HC-SR04-based obstacle detection with programmed braking and turning logic.
- Custom 3D-printed chassis parts; STL files are included in the repository.

## 2. Hardware, Circuit Design & Bill of Materials

### Circuit and assembly

![Gesture controller circuit diagram](docs/circuit-diagram.png)

![Internal electronics and wiring](docs/PHOTO-2026-10-23-02-08-37.jpg)

The source circuit image is also preserved at [docs/circuit.heic](docs/circuit.heic).

### Priced Bill of Materials (BOM)

The prices below were checked on **23 October 2026** from the linked Indian retailers. Where a retailer listed a pre-tax price, the shown unit price includes 18% GST calculated from that listing. Shipping is not included. Prices and stock can change.

| Item | Specification / selection | Qty. | Unit price (incl. GST) | Line total | Buying link |
|---|---|---:|---:|---:|---|
| ESP32 development board | Generic ESP-WROOM-32-style Wi-Fi/Bluetooth board; exact installed brand not identified | 2 | ₹400.02 | ₹800.04 | [ElectronicsComp](https://www.electronicscomp.com/esp32-development-board-with-wifi-bluetooth-india) |
| IMU gesture sensor | MPU-9250 9-axis module; match the installed board layout | 1 | ₹442.50 | ₹442.50 | [ElectronicsComp](https://www.electronicscomp.com/mpu9250-9-axis-gyro-accelerometer-module) |
| Motor driver | L298N dual H-bridge module | 1 | ₹186.44 | ₹186.44 | [ElectronicsComp](https://www.electronicscomp.com/l298n-dual-h-bridge-dc-stepper-motor-driver-controller-module) |
| TT/BO geared DC motors | Reference alternative: 6 V, 230 RPM, dual-side shaft; exact installed RPM is not documented. Select this specific variant on the seller's motor listings | 4 | ₹128.00 | ₹512.00 | [Robu.in motor variants](https://robu.in/product-category/straight-bo-motors/) |
| Robot wheels | Reference alternative: 65 mm yellow-rim BO-motor-compatible wheel | 4 | ₹44.84 | ₹179.36 | [ElectronicsComp](https://www.electronicscomp.com/65mm-robot-wheel-for-bo-motors-yellow) |
| Ultrasonic sensor | HC-SR04 rangefinder | 1 | ₹79.00 | ₹79.00 | [Robu.in](https://robu.in/product/hc-sr04-ultrasonic-range-finder/) |
| Battery pack | **Example alternative only:** 7.4 V, 2600 mAh Li-ion pack with BMS. The installed pack's voltage, capacity, connector, and chemistry were not confirmed from the repository | 1 | ₹765.82 | ₹765.82 | [ElectronicsComp](https://www.electronicscomp.com/7.4v-2600mah-li-ion-battery-pack-inbuilt-bms-protection) |
| Jumper wires | 40-piece male-to-female 10 cm pack; an allowance for prototype wiring | 1 pack | ₹63.72 | ₹63.72 | [ElectronicsComp](https://www.electronicscomp.com/male-to-female-jumper-wires-10cm-40-pieces-pack) |

**Reference total for the listed items: ₹3,028.88.** This total includes the example battery pack and the reference motor/wheel selections above. It is a shopping estimate, **not a record of the amount spent on the original prototype**.

**Reference subtotal excluding the example battery pack: ₹2,263.06.**

#### Items not included in the estimate

- 3D-printing material and machine time: the STL files are provided, but filament usage and fabrication cost are not documented.
- Power switch, screws/standoffs, connectors, solder, USB cables, battery charger, and any voltage-divider or level-shifter parts: the exact models/quantities or installed configuration could not be reliably established from the repository.
- Shipping, tools, replacement parts, and any taxes beyond those shown in the source listings.

#### Compatibility and safety notes

- Confirm the voltage, current rating, shaft type, and RPM of the motors in your own build before ordering substitute motors or wheels. The reference products above are not claimed to be the exact purchased parts.
- A typical HC-SR04 ECHO output may be 5 V, while ESP32 GPIOs are not 5 V tolerant. Use a suitable voltage divider or logic-level shifter before connecting ECHO to GPIO 19 if the installed sensor outputs 5 V.
- Check battery chemistry, voltage, discharge current, connector polarity, and charger compatibility before purchase. Do not assume the example battery is a drop-in replacement.
- The L298N has a meaningful voltage drop and dissipates heat; verify motor current and driver thermal limits before operating.

## 3. System Architecture & Firmware

### Repository structure

```text
Gesture-Controlled-Car/
├── GestureController/
│   └── GestureController.ino
├── GestureCarReceiver/
│   └── GestureCarReceiver.ino
├── 3D Cad/
├── docs/
│   ├── circuit-diagram.png
│   ├── circuit.heic
│   ├── PHOTO-2026-10-23-01-56-13.jpg
│   ├── PHOTO-2026-10-23-02-08-37.jpg
│   └── VIDEO-2026-10-23-02-05-10.mp4
├── hardware_headers/
├── JOURNAL.md
├── BOM.csv
└── README.md
```

### Controller-to-car flow

1. The MPU-9250 supplies accelerometer readings to the controller ESP32 over I2C.
2. The controller calculates pitch and roll and maps the tilt to a command ID.
3. The controller sends a `comand_id` packet over ESP-NOW approximately every 100 ms.
4. The receiver switches the motor driver outputs according to the incoming command.
5. The receiver checks the HC-SR04 distance and runs its programmed obstacle-response routine when the measured distance is at or below the configured 20 cm threshold.

### Controller wiring (I2C)

| ESP32 controller pin | MPU-9250 pin | Function |
|---|---|---|
| 3V3 | VCC | Sensor power, according to the installed module requirements |
| GND | GND | Common ground |
| GPIO 14 | SCL | I2C clock |
| GPIO 15 | SDA | I2C data |

### Receiver GPIO map

| Signal | ESP32 GPIO | Function |
|---|---:|---|
| LIN1 | 26 | Left motor direction input 1 |
| LIN2 | 25 | Left motor direction input 2 |
| RIN1 | 17 | Right motor direction input 1 |
| RIN2 | 16 | Right motor direction input 2 |
| PWMA | 32 | Left motor PWM output |
| PWMB | 33 | Right motor PWM output |
| TRIG_PIN | 18 | HC-SR04 trigger |
| ECHO_PIN | 19 | HC-SR04 echo input |

### Command reference

| Command ID | Action | Controller gesture |
|---:|---|---|
| 0 | Forward | Pitch greater than +15° |
| 1 | Left | Roll less than −15° |
| 2 | Right | Roll greater than +15° |
| 3 | Brake | Tilt within the neutral threshold |
| 4 | Reverse toggle | Pitch less than −15° |

**Known firmware limitation:** the controller sends reverse command 4 repeatedly while the backward-tilt gesture is held, and the receiver toggles its reverse state on every received command 4. Add edge detection/debounce and physically test it before relying on reverse mode.

### Setup

1. Install Arduino IDE and Espressif ESP32 board support.
2. Install the `MPU9250_asukiaaa` library for the gesture controller.
3. Read the receiver ESP32 MAC address using `WiFi.macAddress()` on the receiver.
4. Replace the placeholder `carMAC` array in `GestureController/GestureController.ino` with the actual receiver MAC address.
5. Upload `GestureController/GestureController.ino` to the controller ESP32.
6. Upload `GestureCarReceiver/GestureCarReceiver.ino` to the car ESP32.

The receiver code uses the ESP32 Arduino Core v2.x LEDC API (`ledcSetup`, `ledcAttachPin`, `ledcWrite`). If using Core v3.x, adapt the PWM attachment calls according to the installed core API.

## 4. Testing & Engineering Lessons

### Validation status

| Test | Status / evidence |
|---|---|
| Physical assembly and packaging | Photos are included in `docs/`. |
| Driving demonstration | A demo video is included in `docs/`; review the clip to understand what it demonstrates. |
| Firmware compilation | Do not treat source inspection as proof of a successful build; record the exact board/core version and compile result if available. |
| ESP-NOW range and reliability | Maximum range and packet reliability have not been measured in the repository. |
| Ultrasonic obstacle avoidance | Logic is implemented; repeatable bench/field test logs are not documented. |
| Reverse toggle stability | Needs physical validation and preferably command-edge debounce. |

### Practical validation checklist

- [ ] Verify battery polarity and regulated voltage rails before powering the controller and receiver.
- [ ] Check that the MPU-9250 is detected over I2C.
- [ ] Verify that the controller is configured with the receiver's actual MAC address.
- [ ] Elevate the drive wheels and test each direction at low speed before ground driving.
- [ ] Verify ECHO voltage is safe for ESP32 GPIO 19.
- [ ] Test the stop/avoidance behavior with a safe obstacle in a controlled area.
- [ ] Hold the reverse gesture and check whether repeated packets toggle the direction unexpectedly.

### Engineering observations

- The L298N's bipolar output stage drops voltage and produces heat; ensure the motor and battery combination leaves adequate voltage and current headroom.
- The reverse command is a toggle rather than a one-shot event; a transition-based command or receiver-side debounce would be more predictable.
- `pulseIn(ECHO_PIN, HIGH, 30000)` can block the receiver loop for up to 30 ms when an echo is not received. A non-blocking sensor routine is a possible future improvement.

## 5. CAD, Future Improvements & Project Information

### 3D CAD files

The [3D Cad folder](3D%20Cad/) contains STL mesh exports for the custom chassis parts. STL files are useful for printing, but they do not preserve a parametric feature tree. Editable source formats such as STEP, F3D, FCStd, or SCAD should be added if the original designs are available. Fabrication cost is not included in the BOM total.

### Possible future improvements

- Add edge detection/debounce for reverse commands.
- Make ultrasonic sampling non-blocking.
- Send reverse telemetry or obstacle warnings back to the controller.
- Consider a more efficient motor driver after confirming motor stall current and supply requirements.

### Contributing

Suggestions and bug reports are welcome. Please explain the proposed change, test any firmware changes on real hardware, and open a pull request.

### License

No formal open-source license file was found when this README was prepared. Until a license is added, assume all rights are reserved; do not assume the repository is open-source licensed.

---

**Price-check date:** 23 October 2026. Product prices are retailer listings and may change. Confirm availability, taxes, shipping, connector compatibility, and electrical ratings before purchasing.
