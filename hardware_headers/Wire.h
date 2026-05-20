#pragma once
// Minimal stub for Wire.h

class TwoWire {
public:
  void begin(int sda, int scl) {}
};

extern TwoWire Wire;
