// Include libs
#include "EspMQTTClient.h"
#include <Servo.h>Servo servo;

void onConnectionEstablished();
EspMQTTClient client(
  "Isak",           // Wifi ssid
  "Arduino123",           // Wifi password
  "maqiatto.com",  // MQTT broker ip           // MQTT broker port
  "isak.fogelberg@abbindustrigymnasium.se",            // MQTT username
  "apaapa",       // MQTT password
  "Microdator",          // Client name
  1883
);

#define motorPinRightDir  0//D2
#define motorPinRightSpeed 5//D1

void setup() {
  // put your setup code here, to run once:
  servo.attach(14); //D5
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);
  Serial.begin(115200);
  servo.write(90);
}

void drive(bool dir, int speed) {
  // Set direction of motor to dir
  digitalWrite(motorPinRightDir, dir);
  // Set speed to speed
  analogWrite(motorPinRightSpeed, speed);
}

void onConnectionEstablished()
{
  client.subscribe("isak.fogelberg@abbindustrigymnasium.se/drive", [] (const String & payload)
  {
    // Set toDo to the first letter of payload
    String toDo = payload.substring(0, 1);
    // Set SpeedOrAngle to the 2nd letter to the last letter of payload
    int SpeedOrAngle = payload.substring(1, payload.length()).toInt();
    // Print to serial monitor
    Serial.println(toDo);
    Serial.println(SpeedOrAngle);
    if (toDo == "f") {
      // drive forward with the speed of "SpeedOrAngle"
      drive(false, SpeedOrAngle);
    }
    else if (toDo == "b") {
      // drive backwards with the speed of "SpeedOrAngle"
      drive(true, SpeedOrAngle);
    }
    else if (toDo == "r") {
      // Steer right with the steer angle of SpeedOrAngle
      servo.write(90+SpeedOrAngle);
    }
    else if (toDo == "l") {
      // 
      servo.write(90-SpeedOrAngle);
    }
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
}
