#include <Arduino.h>
#include "Display.h"

#define IR_PIN_FRONT_LEFT 2
#define IR_PIN_FRONT_RIGHT 4
#define IR_PIN_REAR_LEFT 7
#define IR_PIN_REAR_RIGHT 9

#define IR_DIST 15 // centimeters distance if IR detects obstacle


class Obstructions
{
public:
  void init ();
  unsigned long getDistanceBumperLeft();
  unsigned long getDistanceBumperRight();
  unsigned long getDistanceRearBumperLeft();
  unsigned long getDistanceRearBumperRight();
  void scan();

private:
  unsigned long lastLeft;
  unsigned long lastRight;
  unsigned long lastRearLeft;
  unsigned long lastRearRight;
};
