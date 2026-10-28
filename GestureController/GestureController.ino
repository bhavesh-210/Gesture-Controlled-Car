#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>

#define SDA_PIN 15
#define SCL_PIN 14

MPU9250_asukiaaa mpu;

// Command IDs
enum Command {
  FORWARD = 0,
  LEFT_CMD = 1,
  RIGHT_CMD = 2,
  BRAKE_CMD = 3,
  REVERSE_CMD = 4
};

// Message structure sent over ESP-NOW
typedef struct struct_message {
  int comand_id;
} struct_message;

struct_message packet;

// Car receiver MAC address (update with your receiver ESP32 MAC)
uint8_t carMAC[] = { 0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC };

// Function declarations
int detectCommand(float pitch, float roll);
void printCommand(int cmd);

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  mpu.setWire(&Wire);
  mpu.beginAccel();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, carMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Gesture Controller Ready");
}

void loop() {
  mpu.accelUpdate();

  float ax = mpu.accelX();
  float ay = mpu.accelY();
  float az = mpu.accelZ();

  // Calculate tilt angles in degrees
  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180 / PI;
  float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;

  packet.comand_id = detectCommand(pitch, roll);

  esp_now_send(carMAC, (uint8_t*)&packet, sizeof(packet));

  printCommand(packet.comand_id);

  delay(100);
}

int detectCommand(float pitch, float roll) {
  if (pitch > 15)  return FORWARD;
  if (pitch < -15) return REVERSE_CMD;
  if (roll > 15)   return RIGHT_CMD;
  if (roll < -15)  return LEFT_CMD;
  return BRAKE_CMD;
}

void printCommand(int cmd) {
  switch (cmd) {
    case FORWARD:     Serial.println("FORWARD"); break;
    case LEFT_CMD:    Serial.println("LEFT"); break;
    case RIGHT_CMD:   Serial.println("RIGHT"); break;
    case BRAKE_CMD:   Serial.println("BRAKE"); break;
    case REVERSE_CMD: Serial.println("REVERSE"); break;
  }
}
