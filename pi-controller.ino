#define pulsePin 4
#define motorPinRightDir  0//D2
#define motorPinRightSpeed 5//D1
#include "EspMQTTClient.h" // Version 1.11.1 of EspMQTTClient

void onConnectionEstablished();
EspMQTTClient client(
  "ABB_Gym_IOT",           // Wifi ssid
  "Welcome2abb",           // Wifi password
  "81.229.145.235",  // MQTT broker ip           // MQTT broker port maqiatto.com
  "isak.fogelberg@abbgymnasiet.se",            // MQTT username
  "apaapa",       // MQTT password
  "Microdator",          // Client name
  2883 // 1883
);

int pulseCount = 0;
int lastTime = micros();
int actualSpeed = 0;
int wantedSpeed = 0;
int write = 0;
int error = 0;
int kp = 250;
int ki = 0;
int stop = 0;
int proportional = 0;
int integral = 0;
int dt = 1000000;

// Interrupt
IRAM_ATTR void detectPulse() {
  pulseCount++;
}

bool delayMillis(int time) {
   if (micros()-lastTime >time) {
      lastTime = micros();
      return true;
   }
   else {
    return false;
   }
}
void drive(bool dir, int speed) {
  // Set direction of motor to dir
  digitalWrite(motorPinRightDir, dir);
  // Set speed to speed
  analogWrite(motorPinRightSpeed, speed);
}
void onConnectionEstablished() {
  client.subscribe("isak.fogelberg@abbgymnasiet.se/drive", [] (const String & payload) {
    if (payload == "s") {
      stop = 0;
    }
    else if (payload == "f") {
      stop = 1;
    }
  });
  client.subscribe("isak.fogelberg@abbgymnasiet.se/wantedspeed", [] (const String & payload) {
    wantedSpeed = payload.toInt();
  });
  client.subscribe("isak.fogelberg@abbgymnasiet.se/kp", [] (const String & payload) {
    kp = payload.toInt();
  });
  client.subscribe("isak.fogelberg@abbgymnasiet.se/ki", [] (const String & payload) {
    ki = payload.toInt();
  });
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pulsePin, INPUT);
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pulsePin), detectPulse, RISING);
}
void loop() {
  if (delayMillis(dt)) {
    actualSpeed = (58*pulseCount*10)/48; // 58 is circumfrence/2 2 because there are 2 pulses/wheel length
    Serial.println("pulses " + String(pulseCount));
    Serial.println("k: " + String(kp));
    error = wantedSpeed - actualSpeed;
    proportional = kp*error;
    integral += error*dt;
    write += proportional + integral*ki;
    pulseCount = 0;
    client.publish("isak.fogelberg@abbgymnasiet.se/speed", String(actualSpeed));
  }
  client.loop();
  drive(true, write * stop);
}
