#pragma once
// Minimal stub for MPU9250_asukiaaa.h used by the sketch

class MPU9250_asukiaaa {
public:
  void setWire(void* w) {}
  void beginAccel() {}
  void accelUpdate() {}
  float accelX() { return 0.0f; }
  float accelY() { return 0.0f; }
  float accelZ() { return 1.0f; }
};
