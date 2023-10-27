#include "Obstructions.h"

void Obstructions::init()
{
  pinMode(IR_PIN_FRONT_LEFT, INPUT);
  pinMode(IR_PIN_FRONT_RIGHT, INPUT);
  pinMode(IR_PIN_REAR_LEFT, INPUT);
  pinMode(IR_PIN_REAR_RIGHT, INPUT);
}

void Obstructions::scan() {
  lastLeft = digitalRead(IR_PIN_FRONT_LEFT) == 0 ? IR_DIST : 0; 
  lastRearLeft = digitalRead(IR_PIN_REAR_LEFT) == 0 ? IR_DIST : 0; 
  lastRight = digitalRead(IR_PIN_FRONT_RIGHT) == 0 ? IR_DIST : 0; 
  lastRearRight = digitalRead(IR_PIN_REAR_RIGHT) == 0 ? IR_DIST : 0; 
}

unsigned long Obstructions::getDistanceBumperLeft()
{
  return lastLeft;
}

unsigned long Obstructions::getDistanceBumperRight()
{
  return lastRight;
}

unsigned long Obstructions::getDistanceRearBumperLeft()
{
  return lastRearLeft;
}

unsigned long Obstructions::getDistanceRearBumperRight()
{
  return lastRearRight;
}