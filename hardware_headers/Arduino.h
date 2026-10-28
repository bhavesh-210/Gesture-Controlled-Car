#pragma once
// Minimal stub for Arduino.h to satisfy editor IntelliSense
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Basic pin states
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW  0x0
#endif

#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif

struct SerialClass {
	void begin(int) {}
	void println(const char*) {}
	void println(const int v) {}
};

extern SerialClass Serial;

// Basic timing and memory
inline void delay(unsigned long ms) {}
inline void delayMicroseconds(unsigned int us) {}

// Basic digital I/O
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}

// PWM helpers (ESP32 APIs used in sketches)
inline void ledcSetup(int, int, int) {}
inline void ledcAttachPin(int, int) {}
inline void ledcWrite(int, int) {}

inline unsigned long pulseIn(int /*pin*/, int /*value*/, unsigned long /*timeout*/=0) { return 0; }

typedef unsigned long unsigned_long_t;
