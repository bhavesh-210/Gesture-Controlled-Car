#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Motor driver direction pins (L298N)
const int LIN1 = 26;
const int LIN2 = 25;

const int RIN1 = 17;
const int RIN2 = 16;

// PWM speed control pins (L298N enable pins)
#define PWMA 32
#define PWMB 33

#define PWM_FREQ 1000
#define PWM_RES 8       // 8-bit resolution (0-255)
#define CH_A 0
#define CH_B 1

// HC-SR04 ultrasonic sensor pins
#define TRIG_PIN 18
#define ECHO_PIN 19
#define OBSTACLE_DISTANCE 20   // Distance threshold in cm

// Motor direction state
bool Reversed = false;
int Out1 = HIGH;
int Out2 = LOW;

int motorSpeed = 180;   // Motor driving speed (0-255)

// Structure to receive commands over ESP-NOW
typedef struct struct_message {
  int comand_id;
} struct_message;

struct_message Comand;

// Function declarations
void Forward();
void Left();
void Right();
void Brake();
void Reverse();
void CheckReverse();
long getDistance();
bool obstacleDetected();

void OnDataRecv(const esp_now_recv_info *info,
                const uint8_t *incomingData,
                int len);

void setup() {
  Serial.begin(115200);

  // Set motor direction pins as outputs
  pinMode(LIN1, OUTPUT);
  pinMode(LIN2, OUTPUT);
  pinMode(RIN1, OUTPUT);
  pinMode(RIN2, OUTPUT);

  // Set ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configure PWM for speed control
  ledcSetup(CH_A, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);

  ledcAttachPin(PWMA, CH_A);
  ledcAttachPin(PWMB, CH_B);

  ledcWrite(CH_A, motorSpeed);
  ledcWrite(CH_B, motorSpeed);

  Brake();

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW Ready");
}

void loop() {
  CheckReverse();

  // If an obstacle is detected within 20 cm, brake and turn right
  if (obstacleDetected()) {
    Brake();
    delay(200);
    Right();
    delay(350);
    Brake();
  }
}

// Callback triggered whenever a command packet is received
void OnDataRecv(const esp_now_recv_info *info,
                const uint8_t *incomingData,
                int len) {

  memcpy(&Comand, incomingData, sizeof(Comand));

  switch (Comand.comand_id) {
    case 0: Forward(); break;
    case 1: Left();    break;
    case 2: Right();   break;
    case 3: Brake();   break;
    case 4: Reverse(); break;
    default: Brake();  break;
  }
}

void CheckReverse() {
  if (Reversed) {
    Out1 = LOW;
    Out2 = HIGH;
  } else {
    Out1 = HIGH;
    Out2 = LOW;
  }
}

// Read obstacle distance using HC-SR04
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;

  return duration * 0.034 / 2;
}

bool obstacleDetected() {
  return getDistance() <= OBSTACLE_DISTANCE;
}

void Forward() {
  if (obstacleDetected()) {
    Brake();
    return;
  }

  ledcWrite(CH_A, motorSpeed);
  ledcWrite(CH_B, motorSpeed);

  digitalWrite(LIN1, Out1);
  digitalWrite(LIN2, Out2);
  digitalWrite(RIN1, Out1);
  digitalWrite(RIN2, Out2);
}

void Left() {
  ledcWrite(CH_A, 0);          // Stop left wheels
  ledcWrite(CH_B, motorSpeed); // Drive right wheels

  digitalWrite(LIN1, LOW);
  digitalWrite(LIN2, LOW);
  digitalWrite(RIN1, Out1);
  digitalWrite(RIN2, Out2);
}

void Right() {
  ledcWrite(CH_A, motorSpeed); // Drive left wheels
  ledcWrite(CH_B, 0);          // Stop right wheels

  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, LOW);
  digitalWrite(LIN1, Out1);
  digitalWrite(LIN2, Out2);
}

void Brake() {
  ledcWrite(CH_A, 0);
  ledcWrite(CH_B, 0);

  digitalWrite(LIN1, LOW);
  digitalWrite(LIN2, LOW);
  digitalWrite(RIN1, LOW);
  digitalWrite(RIN2, LOW);
}

void Reverse() {
  Reversed = !Reversed;
}
