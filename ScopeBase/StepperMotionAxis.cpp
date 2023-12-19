#include "StepperMotionAxis.h"

StepperMotionAxis::StepperMotionAxis (String axisId, int directionPin, int pulsePin, int stepsPerRev, float mmPerRev, float axisScaler, int lowerLimitSwitchPin, int upperLimitSwitchPin) {
  stepper = new Stepper(stepsPerRev, pulsePin, directionPin);
  stepper->setSpeed(getSpeedValue(defaultSpeed));
  
  this->axisId = axisId;
  this->stepsPerRev = stepsPerRev;
  this->mmPerRev = mmPerRev;
  this->axisScaler = axisScaler;
  this->lowerLimitSwitchPin = lowerLimitSwitchPin;
  this->upperLimitSwitchPin = upperLimitSwitchPin;
}

float StepperMotionAxis::getPosition () {
  return currentPosition;
}

void StepperMotionAxis::center () {
  goToPosition(maxPosition / 2, AXIS_SPEED_FAST);
}

void StepperMotionAxis::goToPosition (float position, AxisSpeed speed) {
  if (position < minPosition || position > maxPosition) {
    writeLog("Requested position " + String(position) + " is out of bounds!");
  }
  else {
    float scaledAmount = position - currentPosition;
    writeLog("Going to " + String(position) + ", need to adjust current position by " + String(scaledAmount));
    move(scaledAmount, speed);
  }
}

int StepperMotionAxis::getSpeedValue (AxisSpeed speed) {
  switch(speed) {
    case AXIS_SPEED_SLOW:
      return STEPPER_X_SLOW_SPEED;
    case AXIS_SPEED_MEDIUM:
      return STEPPER_X_MEDIUM_SPEED;
    case AXIS_SPEED_FAST:
      return STEPPER_X_FAST_SPEED;
    default:
      return STEPPER_X_SLOW_SPEED;
  }
}


void StepperMotionAxis::move (float scaledAmount, AxisSpeed speed) {
  stepper->setSpeed(getSpeedValue(speed));
  float scaledDistance = abs(scaledAmount);
  if (currentPosition + scaledAmount < minPosition || currentPosition + scaledAmount > maxPosition) {
    writeLog("Requested movement amount " + String(scaledAmount) + " is out of bounds!");
  }
  else {
    int thisStepSizeScaled = smallStepSizeScaled;
    writeLog("Moving by " + String(scaledAmount));
    float revsNeeded = scaledDistance / mmPerRev;
    int scaledSteps = revsNeeded / thisStepSizeScaled;
    //int scaledSteps = scaledDistance / thisStepSizeScaled;
    float fractionalStep = (scaledDistance - (scaledSteps * mmPerRev)) / (mmPerRev * scaledSteps);
    int directionMultiplier = (scaledAmount > 0) ? 1 : -1;

    writeLog("Scaled steps: " + String(scaledSteps) + ", last fractional step: " + String(fractionalStep));

    for (int stepCount = 0; stepCount < scaledSteps; stepCount++) {
      writeLog("Running step " + String(stepCount) + ", Rotations: " + String(thisStepSizeScaled) + ", Steps: " + String(stepsPerRev * thisStepSizeScaled));
      stepper->step(directionMultiplier * stepsPerRev * thisStepSizeScaled);
      delay(10);
    }

    if (fractionalStep != 0) {
      stepper->step(directionMultiplier * fractionalStep * thisStepSizeScaled * stepsPerRev);
    }

    currentPosition = currentPosition + scaledAmount;
  }
}

void StepperMotionAxis::calibrate () {

  writeLog("Calibrating");
  stepper->setSpeed(getSpeedValue(AXIS_SPEED_SLOW));


  writeLog("Finding upper limit");
  while(digitalRead(upperLimitSwitchPin) != LOW) {
    delay(10);
    stepper->step(stepsPerRev * smallStepSizeScaled);
  }
  maxPosition = 0.0; // this will be calculated while finding lower limit
  writeLog("Upper limit found");


  writeLog("Finding lower limit");
  int stepCount = 0;
  while(digitalRead(lowerLimitSwitchPin) != LOW) {
    delay(10);
    stepper->step(-1 * stepsPerRev * smallStepSizeScaled);
    writeLog("Step count " + String(stepCount) + ", Stepping " + String(-1 * stepsPerRev * smallStepSizeScaled) + ", pre max amount: " + String(maxPosition));
    maxPosition += (smallStepSizeScaled * mmPerRev);
    stepCount++;
  }
  currentPosition = 0;

  writeLog("Lower limit found");
  writeLog("Min: " + String(minPosition) + ", Max: " + String(maxPosition) + ", Current: " + String(currentPosition));
  stepper->setSpeed(getSpeedValue(defaultSpeed));
}

void StepperMotionAxis::writeLog (String msg) {
  if (LOG_ENABLED)
    Serial.println(axisId + "-Axis -> " + msg);
}