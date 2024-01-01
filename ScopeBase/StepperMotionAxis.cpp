#include "StepperMotionAxis.h"

StepperMotionAxis::StepperMotionAxis (String axisId, int directionPin, int pulsePin, int stepsPerRev, float mmPerRev, float axisScaler, int lowerLimitSwitchPin, int upperLimitSwitchPin, int slowSpeed, int mediumSpeed, int fastSpeed) {
  stepper = new Stepper(stepsPerRev, pulsePin, directionPin);
  stepper->setSpeed(getSpeedValue(defaultSpeed));
  
  this->axisId = axisId;
  this->stepsPerRev = stepsPerRev;
  this->mmPerRev = mmPerRev;
  this->axisScaler = axisScaler;
  this->lowerLimitSwitchPin = lowerLimitSwitchPin;
  this->upperLimitSwitchPin = upperLimitSwitchPin;
  this->stepperSlow = slowSpeed;
  this->stepperMedium = mediumSpeed;
  this->stepperFast = fastSpeed;
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
      return this->stepperSlow;
    case AXIS_SPEED_MEDIUM:
      return this->stepperMedium;
    case AXIS_SPEED_FAST:
      return this->stepperFast;
    default:
      return this->stepperSlow;
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
  stepper->setSpeed(getSpeedValue(AXIS_SPEED_MEDIUM));


  writeLog("Finding upper limit");
  bool upperFound = false;
  while(!upperFound) {
    delay(10);
    stepper->step(stepsPerRev * smallStepSizeScaled);
    // check if the limit switch is closed
    if (digitalRead(upperLimitSwitchPin) == LOW) {
      // wait and check again. occasionally get false reading
      delay(100);
      upperFound = digitalRead(upperLimitSwitchPin) == LOW;
    }
  }
  maxPosition = 0.0; // this will be calculated while finding lower limit
  writeLog("Upper limit found");


  writeLog("Finding lower limit");
  bool lowerFound = false;
  //int stepCount = 0;
  while(!lowerFound) {
    delay(10);
    stepper->step(-1 * stepsPerRev * smallStepSizeScaled);
    //writeLog("Step count " + String(stepCount) + ", Stepping " + String(-1 * stepsPerRev * smallStepSizeScaled) + ", pre max amount: " + String(maxPosition));
    maxPosition += (smallStepSizeScaled * mmPerRev);

    // check if the limit switch is closed
    if (digitalRead(lowerLimitSwitchPin) == LOW) {
      // wait and check again. occasionally get false reading
      delay(100);
      lowerFound = digitalRead(lowerLimitSwitchPin) == LOW;

    }
    //stepCount++;
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