---
title: "Gesture-Controlled Car — The Nonchalant"
github: "bhavesh-210/Gesture-Controlled-Car"
description: "Build notes for a gesture-controlled ESP32 robot car with a custom chassis"
total_time_spent: "21h"
---

# Building The Nonchalant — my project journal

**Time recorded in the original journal:** 21 hours across five stages.

This is the story behind my gesture-controlled car, **The Nonchalant**. I wanted to control a small robot without tapping buttons on a phone or holding a traditional remote. Tilting my hand felt like a fun way to explore motion sensors, and the idea gradually turned into a full project involving embedded programming, electronics, wireless communication, and CAD.

I'm keeping this journal as an honest record of the approach, the decisions in the code, and the rough edges that are still there. The time breakdown below comes from the previous version of my journal; it isn't a set of exact calendar timestamps or a claim that I kept minute-by-minute records.

![Finished car](docs/PHOTO-2026-10-23-01-56-13.jpg)

## Stage 1 — Starting with the idea

**Time spent: 2h 30m**

The first question was how to make the car understand a hand movement. I didn't want to build a full gesture-recognition or camera system for this prototype. I only needed a small set of reliable directions. An IMU seemed like a reasonable fit because it can measure acceleration along three axes, which is enough to estimate how far the hand is tilted when it is relatively still.

I chose two ESP32 boards: one on the hand controller and one in the car. They communicate using ESP-NOW, which lets the boards exchange compact packets directly without needing a router. That also meant I could focus on the embedded side rather than building an app first.

My initial plan was simple: read the controller's tilt, turn it into a direction, send that direction to the car, and let the car drive its wheels. I also wanted some basic obstacle awareness so a forward command wouldn't always mean driving straight into whatever was in front of it.

I settled on five commands: forward, left, right, brake, and a reverse-mode toggle. The project would use four geared motors, two on each side, so steering could be done by controlling the left and right sides separately.

## Stage 2 — Parts, wiring, and fitting everything together

**Time spent: 5h**

Choosing parts was a balance between cost, ease of use, and whether I could actually connect everything without making the project unnecessarily complicated.

I used two ESP-WROOM-32 development boards, an MPU-9250 IMU, an L298N driver, four 6 V TT/BO motors and wheels, and an HC-SR04 distance sensor. The car uses a 7.4 V rechargeable battery pack. I collected the parts and estimated prices in [BOM.csv](BOM.csv); the list is more useful for replicating the build than copying prices into every section of the journal.

![Controller wiring diagram](docs/circuit-diagram.png)

For the hand controller, I connected the IMU to the ESP32 over I2C. In the sketch, SDA is on GPIO 15 and SCL is on GPIO 14. The car has more connections: four GPIO pins control motor direction, two PWM pins set the left and right motor speeds, and two pins connect to the ultrasonic sensor.

![Electronics inside the chassis](docs/PHOTO-2026-10-23-02-08-37.jpg)

One hardware detail that matters is the HC-SR04 echo voltage. A typical 5 V module can send a 5 V echo pulse, which must be level-shifted before it reaches an ESP32's 3.3 V GPIO. The L298N also loses some voltage internally, so the voltage available to the motors is lower than the battery voltage. Those are the sorts of details that are easy to miss when looking at a wiring diagram but important on a real build.

This stage also made me think about cable routing and where the electronics would sit. A car can work electrically on a table and still be awkward once the motors, battery, and wires all have to share the same chassis.

## Stage 3 — Writing the controller and receiver code

**Time spent: 6h 30m**

I divided the code into two Arduino sketches so that each board has a clear job:

- [`GestureController/GestureController.ino`](GestureController/GestureController.ino) reads the sensor, decides the gesture, and sends an ESP-NOW packet.
- [`GestureCarReceiver/GestureCarReceiver.ino`](GestureCarReceiver/GestureCarReceiver.ino) receives the packet, drives the motor outputs, and checks the distance sensor.

### Turning tilt into commands

The hand controller reads acceleration on the X, Y, and Z axes and uses `atan2` to estimate pitch and roll:

```cpp
float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI;
float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
```

The next part is deliberately simple. The sketch checks whether pitch or roll passes a 15-degree threshold. Positive pitch means forward, negative pitch means the reverse toggle, positive roll means right, and negative roll means left. If no threshold is crossed, it sends brake. Pitch is checked before roll, so it takes priority if both are beyond their limits.

Every 100 ms, the transmitter sends a packet containing an integer command ID. Both sketches use the same message structure, including the field name `comand_id` as written in the source. The spelling is not ideal, but the two sides agree on it.

### Making the wheels respond

The receiver maps command IDs to functions such as `Forward()`, `Left()`, `Right()`, and `Brake()`. Rather than using a steering mechanism, it stops the motor pair on one side and drives the other pair to turn.

I set motor PWM to 1 kHz with 8-bit resolution and a fixed speed value of `180` on the 0–255 scale. This gives a basic, repeatable speed setting. It does not yet translate a larger hand tilt into a faster motor speed.

Reverse works as a mode. Receiving command `4` flips a boolean flag; the `CheckReverse()` helper then changes the motor-direction pin state used by subsequent movement. This is one of the design choices I'd revisit because sending a toggle over and over is not the same as sending a stable reverse state.

### Adding the ultrasonic check

The HC-SR04 sends a short trigger pulse, then the receiver measures the echo pulse duration to estimate distance. The threshold in the sketch is **20 cm**. A forward command checks for an obstacle before driving; the receiver's main loop also watches for nearby objects. On detection it brakes, waits 200 ms, turns right for 350 ms, then brakes again.

That behavior is intentionally basic. It doesn't choose the clearest route or estimate where the obstacle is moving. It just adds an immediate response to something detected in front of the car.

## Stage 4 — Trying it out and finding the awkward parts

**Time spent: 4h**

This was the part of the project that made the gap between a simple design and a useful prototype more obvious. There are several different things to check: whether the hand sensor gives sensible readings, whether packets reach the other ESP32, whether the motor directions match the gestures, and how the car behaves once it is carrying its own weight.

I kept a demonstration video and photos in the repository:

- [Driving demonstration](docs/VIDEO-2026-10-23-02-05-10.mp4)
- [Assembled car](docs/PHOTO-2026-10-23-01-56-13.jpg)
- [Inside the chassis](docs/PHOTO-2026-10-23-02-08-37.jpg)

The current implementation also makes a few issues clear, even without pretending they have all been solved:

**A held reverse gesture can toggle too many times.** The transmitter sends the same command roughly ten times a second while the hand stays tilted back. The receiver treats every one as a fresh request to switch directions. I would fix this with a state change or debounce rule so a single gesture produces a single toggle.

**The car should stop if wireless updates stop.** The receiver handles incoming commands but doesn't keep a timeout for the last packet. A safer version should set the motor outputs to stop after a short period without a valid command. That is an important missing feature, not just an optional extra.

**The obstacle routine blocks other work.** The ultrasonic read can wait for an echo, and the stop-and-turn sequence uses `delay()`. That approach is easy to understand, but it isn't ideal when the car also needs to react promptly to new commands.

**The ESP32 library version needs attention.** The current receiver sketch combines the older `ledcSetup`/`ledcAttachPin` PWM functions with a newer-style ESP-NOW receive callback. Depending on the installed Arduino-ESP32 core, this may not compile as-is. The next cleanup should choose one supported core version and adapt both pieces consistently.

These are practical limitations I would rather document than hide behind a claim that the project is completely finished.

## Stage 5 — Designing the chassis and deciding what comes next

**Time spent: 3h**

![3D chassis preview](docs/car-3d-model.svg)

I didn't want the project to remain a collection of boards and loose wires. I designed a chassis in Fusion 360 so that the motor mounts, wheels, battery, and control electronics had defined places.

The [`3D Cad/`](3D%20Cad/) directory includes a Fusion 360 `.f3d` design archive and several STEP models for the chassis elements. Keeping editable source files matters because the design may need small changes if someone uses a different battery, motor, or mounting layout. STEP is useful for sharing solid geometry, although a slicer may require a mesh export before printing.

The assembled frame makes the car feel like one project instead of separate electronics and software exercises. It also showed why mechanical design belongs in the development process, not just at the end.

### What I'd improve in another version

The highest priority would be reliability: a wireless-command timeout, a proper one-shot reverse toggle, and an obstacle routine that doesn't block the receiver for fixed periods. After that I'd work on proportional steering, smoother speed changes, and a more efficient motor driver. Two-way communication could also let the controller know about a low battery or an obstacle event.

## Looking back

The most useful lesson from The Nonchalant is how interconnected the parts are. A movement that looks easy in code depends on the IMU's orientation, the command logic, packet delivery, motor power, and the weight and shape of the chassis. Each decision affects the behavior of the whole car.

The repository captures a working direction for the project and documents where the current implementation still needs attention. I want someone opening it to understand not only **what I built**, but also **how it works, why I made these choices, and what I would fix next**.

**Total time carried over from the original log: 21 hours** (2h 30m + 5h + 6h 30m + 4h + 3h).

— **Bhavesh Sharma** · [Project repository](https://github.com/bhavesh-210/Gesture-Controlled-Car)
