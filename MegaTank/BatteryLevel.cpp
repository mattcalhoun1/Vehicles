#include "BatteryLevel.h"

/*int value = 0;
float voltage;
float R1 = 47000.0;
float R2 = 33000.0;
*/

/*void loop(){
  value = analogRead(A0);
  voltage = value * (5.0/1024)*((R1 + R2)/R2);
  Serial.print("Voltage =");
  Serial.println(voltage);
  delay(500);
}
*/


BatteryLevel::BatteryLevel(int analogPin) {
  voltageMeterPin = analogPin;
}

float BatteryLevel::getLevel() {
  int voltageValue = analogRead(voltageMeterPin);
  //voltage = value * (5.0/1024)*((R1 + R2)/R2);
  return float(voltageValue/5);
}
  
