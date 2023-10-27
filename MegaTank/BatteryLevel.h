#include <Arduino.h>

class BatteryLevel {
  public:
    BatteryLevel(int analogPin);
    float getLevel();
  
  private:
    int voltageMeterPin;
};