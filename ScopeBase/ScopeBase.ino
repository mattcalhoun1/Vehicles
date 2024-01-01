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
  STEPPER_X_UPPER_LIMIT_PIN,
  STEPPER_X_SLOW_SPEED,
  STEPPER_X_MEDIUM_SPEED,
  STEPPER_X_FAST_SPEED);

StepperMotionAxis yAxis = StepperMotionAxis (
  AXIS_NAME_Y, 
  STEPPER_Y_DIRECTION_PIN,
  STEPPER_Y_PULSE_PIN,
  STEPPER_Y_STEPS_PER_REV,
  STEPPER_Y_MM_PER_REV,
  AXIS_SCALER_Y,
  STEPPER_Y_LOWER_LIMIT_PIN,
  STEPPER_Y_UPPER_LIMIT_PIN,
  STEPPER_Y_SLOW_SPEED,
  STEPPER_Y_MEDIUM_SPEED,
  STEPPER_Y_FAST_SPEED);

bool yMotion = true;
bool xMotion = true;

void setup() {
  Serial.begin(9600);

  pinMode(STEPPER_X_LOWER_LIMIT_PIN, INPUT_PULLUP);
  pinMode(STEPPER_X_UPPER_LIMIT_PIN, INPUT_PULLUP);

  pinMode(STEPPER_Y_LOWER_LIMIT_PIN, INPUT_PULLUP);
  pinMode(STEPPER_Y_UPPER_LIMIT_PIN, INPUT_PULLUP);

  if (xMotion) {
    xAxis.calibrate();
    xAxis.center();
  }

  if (yMotion) {
    yAxis.calibrate();
    yAxis.center();
  }
}

void loop() {
  if (xMotion) {
    float xPosition = 50.0;
    Serial.println("X slow move"); //controls the direction of the stepper motor;
    xAxis.move (10.0, AXIS_SPEED_SLOW);
    delay(500);
    xAxis.move (-10.0, AXIS_SPEED_SLOW);
    delay(500);

    Serial.println("X med move"); //controls the direction of the stepper motor;
    xAxis.move (10.0, AXIS_SPEED_MEDIUM);
    delay(500);
    xAxis.move (-10.0, AXIS_SPEED_MEDIUM);
    delay(500);

    Serial.println("X fast move"); //controls the direction of the stepper motor;
    xAxis.move (10.0, AXIS_SPEED_FAST);
    delay(500);
    xAxis.move (-10.0, AXIS_SPEED_FAST);
    delay(500);
  }

  if (yMotion) {
    float yPosition = 50.0;
    Serial.println("Y slow move"); //controls the direction of the stepper motor;
    yAxis.move (10.0, AXIS_SPEED_SLOW);
    delay(500);
    yAxis.move (-10.0, AXIS_SPEED_SLOW);
    delay(500);

    Serial.println("Y med move"); //controls the direction of the stepper motor;
    yAxis.move (10.0, AXIS_SPEED_MEDIUM);
    delay(500);
    yAxis.move (-10.0, AXIS_SPEED_MEDIUM);
    delay(500);

    Serial.println("Y fast move"); //controls the direction of the stepper motor;
    yAxis.move (10.0, AXIS_SPEED_FAST);
    delay(500);
    yAxis.move (-10.0, AXIS_SPEED_FAST);
    delay(500);
  }

}

void writeLog (String msg) {
  Serial.println(msg);
  //delay(500);
}