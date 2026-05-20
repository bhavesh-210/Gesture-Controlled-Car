#pragma once
// Minimal stub for WiFi.h

#ifndef WIFI_STA
#define WIFI_STA 1
#endif

struct WiFiClass {
  void mode(int) {}
};

extern WiFiClass WiFi;
