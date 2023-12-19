#include <Arduino.h>
#include <Stepper.h>
#include "MotionAxis.h"
#include "Globals.h"

#ifndef STEPPER_MOTION_AXIS_H
#define STEPPER_MOTION_AXIS_H

class StepperMotionAxis : MotionAxis {
  public:
    StepperMotionAxis (String axisId, int directionPin, int pulsePin, int stepsPerRev, float mmPerRev, float axisScaler, int lowerLimitSwitchPin, int upperLimitSwitchPin);
    virtual void goToPosition (float position, AxisSpeed speed);
    virtual void move (float scaledAmount, AxisSpeed speed);
    virtual void calibrate ();
    virtual float getPosition ();
    virtual void center ();

  private:
    Stepper* stepper;
    String axisId;
    int stepsPerRev;
    float mmPerRev;
    float axisScaler;
    int bigStepSizeScaled = STEPPER_BIG_STEP_SIZE_SCALED;
    int smallStepSizeScaled = STEPPER_SMALL_STEP_SIZE_SCALED;
    int lowerLimitSwitchPin;
    int upperLimitSwitchPin;

    // must be calibrated before these are accurate
    float currentPosition = 0.0; 
    float minPosition = 0.0;
    float maxPosition = 100.0;

    int getSpeedValue (AxisSpeed speed);
    AxisSpeed defaultSpeed = AXIS_SPEED_MEDIUM;

    void writeLog (String msg);
};

#endif