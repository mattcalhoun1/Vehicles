#include <Stepper.h>
#include "Globals.h"
#include "StepperMotionAxis.h"

StepperMotionAxis xAxis = StepperMotionAxis (
  AXIS_NAME_X, 
  STEPPER_X_DIRECTION_PIN,
  STEPPER_X_PULSE_PIN,
  STEPPER_X_STEPS_PER_REV,
  STEPPER_X_MM_PER_REV,
  AXIS_SCALER_X,
  STEPPER_X_LOWER_LIMIT_PIN,
  STEPPER_X_UPPER_LIMIT_PIN);

void setup() {
  Serial.begin(9600);

  pinMode(STEPPER_X_LOWER_LIMIT_PIN, INPUT_PULLUP);
  pinMode(STEPPER_X_UPPER_LIMIT_PIN, INPUT_PULLUP);
  //writeLog("attaching interrupts");
  //attachInterrupt(digitalPinToInterrupt(limitSwitchPort), limitFound, LOW);
  //writeLog("interrupts attached");
  xAxis.calibrate();
  xAxis.center();
}

void loop() {
  float xPosition = 50.0;

  //slideX (true, 40);

  Serial.println("slow move"); //controls the direction of the stepper motor;
  xAxis.move (10.0, AXIS_SPEED_SLOW);
  delay(500);
  xAxis.move (-10.0, AXIS_SPEED_SLOW);
  delay(500);

  Serial.println("med move"); //controls the direction of the stepper motor;
  xAxis.move (10.0, AXIS_SPEED_MEDIUM);
  delay(500);
  xAxis.move (-10.0, AXIS_SPEED_MEDIUM);
  delay(500);

  Serial.println("fast move"); //controls the direction of the stepper motor;
  xAxis.move (10.0, AXIS_SPEED_FAST);
  delay(500);
  xAxis.move (-10.0, AXIS_SPEED_FAST);
  delay(500);

}

void writeLog (String msg) {
  Serial.println(msg);
  //delay(500);
}