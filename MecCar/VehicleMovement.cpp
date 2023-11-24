#include "VehicleMovement.h"

VehicleMovement::VehicleMovement () {
  motors[LEFT_REAR].EncoderAIn = 28; // was 2
  motors[LEFT_REAR].EncoderPinOut = 4;//4
  motors[LEFT_REAR].OutputPin1 = 6; //6
  motors[LEFT_REAR].OutputPin2 = 5; //5

  motors[LEFT_REAR].GearRatio = 30; // was 150
  motors[LEFT_REAR].TrackLength = 23.0;
  motors[LEFT_REAR].DriveWheelDiameter = 3.75;
  motors[LEFT_REAR].PulsesPerRevolution = 6;
  motors[LEFT_REAR].OutputMin = MIN_MOTOR_OUTPUT;
  motors[LEFT_REAR].OutputMax = MAX_MOTOR_OUTPUT; // up to 255
  motors[LEFT_REAR].WheelRow = 1; // secnd row
  motors[LEFT_REAR].WheelSide = SIDE_LEFT; // left

  motors[RIGHT_REAR].EncoderAIn = 24; // was 3
  motors[RIGHT_REAR].EncoderPinOut = 9; //9
  motors[RIGHT_REAR].OutputPin1 = 7; //7
  motors[RIGHT_REAR].OutputPin2 = 8; //8

  motors[RIGHT_REAR].GearRatio = 30;
  motors[RIGHT_REAR].TrackLength = 23.0;
  motors[RIGHT_REAR].DriveWheelDiameter = 3.75;
  motors[RIGHT_REAR].PulsesPerRevolution = 6;
  motors[RIGHT_REAR].OutputMin = MIN_MOTOR_OUTPUT;
  motors[RIGHT_REAR].OutputMax = MAX_MOTOR_OUTPUT;
  motors[RIGHT_REAR].WheelRow = 1; // second row
  motors[RIGHT_REAR].WheelSide = SIDE_RIGHT; // right

  motors[LEFT_FRONT].EncoderAIn = 18;
  motors[LEFT_FRONT].EncoderPinOut = 33; // was 23
  motors[LEFT_FRONT].OutputPin1 = 31;//27; // was 27 : 31
  motors[LEFT_FRONT].OutputPin2 = 29;//25; // was 25 : 29

  motors[LEFT_FRONT].GearRatio = 30;
  motors[LEFT_FRONT].TrackLength = 23.0;
  motors[LEFT_FRONT].DriveWheelDiameter = 3.75;
  motors[LEFT_FRONT].PulsesPerRevolution = 6;
  motors[LEFT_FRONT].OutputMin = MIN_MOTOR_OUTPUT;
  motors[LEFT_FRONT].OutputMax = MAX_MOTOR_OUTPUT;
  motors[LEFT_FRONT].WheelRow = 0; // first row
  motors[LEFT_FRONT].WheelSide = SIDE_LEFT; // left

  motors[RIGHT_FRONT].EncoderAIn = 19;
  motors[RIGHT_FRONT].EncoderPinOut = 23;  // was 33
  motors[RIGHT_FRONT].OutputPin1 = 25;//29;//29; // was 29
  motors[RIGHT_FRONT].OutputPin2 = 27;//31; // was 31

  motors[RIGHT_FRONT].GearRatio = 30;
  motors[RIGHT_FRONT].TrackLength = 23.0;
  motors[RIGHT_FRONT].DriveWheelDiameter = 3.75;
  motors[RIGHT_FRONT].PulsesPerRevolution = 6;
  motors[RIGHT_FRONT].OutputMin = MIN_MOTOR_OUTPUT;
  motors[RIGHT_FRONT].OutputMax = MAX_MOTOR_OUTPUT;
  motors[RIGHT_FRONT].WheelRow = 0; // first row
  motors[RIGHT_FRONT].WheelSide = SIDE_RIGHT; // right



  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    pinMode(motors[wheel].EncoderPinOut, OUTPUT);
    pinMode(motors[wheel].OutputPin1, OUTPUT);
    pinMode(motors[wheel].OutputPin2, OUTPUT);
  }
	
  // speed / distance tracking
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    tripMeters[wheel] = 0;
    pulseCount[wheel] = 0;
    pulseCountStart[wheel] = 0;
    directionForward[wheel] = true;
    currentSpeeds[wheel] = 0.0;
    targetSpeeds[wheel] = 0.0;
    outputLevels[wheel] = 0;
  }

  // PIds
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    pids[wheel] = new AutoPID (&currentSpeeds[wheel], &targetSpeeds[wheel], &outputLevels[wheel], motors[wheel].OutputMin, motors[wheel].OutputMax, DRIVE_KP, DRIVE_KI, DRIVE_KD); 
    pids[wheel]->setBangBang(15.0);
  }

}

VehicleMovement::~VehicleMovement () {
  // clear the pids
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    pids[wheel]->stop();
  }
}

void VehicleMovement::resetTripMeter () {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    tripMeters[wheel] = 0;
  }
}

void VehicleMovement::strafe (StrafeDirection direction, unsigned long durationMillis) {
  stop();

  bool fl_forward = true;
  bool fr_forward = true;
  bool rl_forward = true;
  bool rr_forward = true;

  bool fl_drive = false;
  bool fr_drive = false;
  bool rl_drive = false;
  bool rr_drive = false;


  switch (direction) {
    case RIGHT_FORWARD:
      fl_drive = true;
      rr_drive = true;
      break;
    case RIGHT_RIGHT:
      fl_drive = true;
      rr_drive = true;
      fr_drive = true;
      fr_forward = false;
      rl_drive = true;
      rl_forward = false;
      break;
    case RIGHT_BACKWARD:
      fr_drive = true;
      fr_forward = false;
      rl_drive = true;
      rl_forward = false;
      break;
    case LEFT_FORWARD:
      fr_drive = true;
      rl_drive = true;
      break;
    case LEFT_LEFT:
      fl_drive = true;
      fl_forward = false;
      rr_drive = true;
      rr_forward = false;

      fr_drive = true;
      rl_drive = true;
      break;
    case LEFT_BACKWARD:
      fl_drive = true;
      fl_forward = false;
      rr_drive = true;
      rr_forward = false;
      break;
  }  

  unsigned long firstWheelPulses = pulseCount[0];
  unsigned long lastWheelPulses = pulseCount[WHEEL_COUNT - 1];

	digitalWrite(motors[LEFT_FRONT].OutputPin1, fl_forward ? HIGH : LOW);
	digitalWrite(motors[LEFT_FRONT].OutputPin2, fl_forward ? LOW : HIGH);
	digitalWrite(motors[LEFT_REAR].OutputPin1, rl_forward ? HIGH : LOW);
	digitalWrite(motors[LEFT_REAR].OutputPin2, rl_forward ? LOW : HIGH);
	digitalWrite(motors[RIGHT_FRONT].OutputPin1, fr_forward ? HIGH : LOW);
	digitalWrite(motors[RIGHT_FRONT].OutputPin2, fr_forward ? LOW : HIGH);
	digitalWrite(motors[RIGHT_REAR].OutputPin1, rr_forward ? HIGH : LOW);
	digitalWrite(motors[RIGHT_REAR].OutputPin2, rr_forward ? LOW : HIGH);

  analogWrite(motors[LEFT_FRONT].EncoderPinOut, STRAFE_MOTOR_OUTPUT);  
  analogWrite(motors[RIGHT_FRONT].EncoderPinOut, STRAFE_MOTOR_OUTPUT);  
  analogWrite(motors[LEFT_REAR].EncoderPinOut, STRAFE_MOTOR_OUTPUT);  
  analogWrite(motors[RIGHT_REAR].EncoderPinOut, STRAFE_MOTOR_OUTPUT);  

  // Wait until all wheels have started moving
  while (pulseCount[0] == firstWheelPulses || pulseCount[WHEEL_COUNT - 1] == lastWheelPulses) {
    delay(20);
  }

  // strafe for given time
  unsigned long start = millis();
  while(millis() - start < durationMillis) {
    delay(20);
  }

  stop();
}

void VehicleMovement::setDirectionSpeed (Wheel wheel, bool forward, float inchesPerSecond) {
  targetSpeeds[wheel] = inchesPerSecond;

  // Pull the setting from the PID
  pids[wheel]->run();

	digitalWrite(motors[wheel].OutputPin1, forward ? HIGH : LOW);
	digitalWrite(motors[wheel].OutputPin2, forward ? LOW : HIGH);

  analogWrite(motors[wheel].EncoderPinOut, getCalculatedOutputLevel(wheel));
  directionForward[wheel] = forward;
}

void VehicleMovement::setDirectionSpeed (bool forward, float inchesPerSecond) {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    setDirectionSpeed((Wheel)wheel, forward, inchesPerSecond);
  }
}

void VehicleMovement::rotate (float degrees) {
  stop();

  unsigned long rotationTime = abs(int(degrees / 90)) * 1000; // assumes rotates 90 degrees per second
  bool leftRotationForward = degrees > 0; // left side will go forward if rotating right
  bool rightRotationForward = degrees < 0; // right side will go forward if rotating left
  unsigned long rotationSpeed = motors[0].OutputMax; // pick a random motor, its max will be our output to all wheels

  unsigned long firstWheelPulses = pulseCount[0];
  unsigned long lastWheelPulses = pulseCount[WHEEL_COUNT - 1];

  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    if (motors[wheel].WheelSide == SIDE_LEFT) {
      // If we are rotating right, this motor should go forward
      digitalWrite(motors[wheel].OutputPin1, leftRotationForward ? HIGH : LOW);
      digitalWrite(motors[wheel].OutputPin2, leftRotationForward ? LOW : HIGH);
    }
    else { // on the right side
      // if we are rotating right, this motor should go backward
      digitalWrite(motors[wheel].OutputPin1, rightRotationForward ? HIGH : LOW);
      digitalWrite(motors[wheel].OutputPin2, rightRotationForward ? LOW : HIGH);
    }
    analogWrite(motors[wheel].EncoderPinOut, rotationSpeed);
  }
  // Wait until all wheels have started moving
  while (pulseCount[0] == firstWheelPulses || pulseCount[WHEEL_COUNT - 1] == lastWheelPulses) {
    delay(20);
  }

  // Now allow the rotation to continue for the given time
  delay(rotationTime);

  stop();
}

void VehicleMovement::stop() {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    pids[wheel]->stop();
    currentSpeeds[wheel] = 0.0;
    analogWrite(motors[wheel].EncoderPinOut, 0);
  	digitalWrite(motors[wheel].OutputPin1, LOW);
  	digitalWrite(motors[wheel].OutputPin2, LOW);
  }
}

int VehicleMovement::getWheelCount() {
  return WHEEL_COUNT;
}


int VehicleMovement::getEncoderAPin (Wheel wheel) {
  return motors[wheel].EncoderAIn;
}

void VehicleMovement::interrupt(Wheel wheel) {
  pulseCount[wheel]++;
}

bool VehicleMovement::updateSpeeds() {
  unsigned long now = millis();
  if ((now - lastSpeedUpdate) > DRIVE_READ_DELAY) {
    lastSpeedUpdate = millis();

    for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
      currentSpeeds[wheel] = getCurrentSpeed((Wheel)wheel);

      // bump the trip meter
      tripMeters[wheel] += getDistance((Wheel)wheel);//(directionForward[wheel] ? 1.0 : -1.0) * getDistance(wheel);

      // reset the pulse count
      resetPulses((Wheel)wheel);
      logConsole("----> Wheel " + String(wheel) + " Speed: " + String(currentSpeeds[wheel]) + " inch/sec");

      // update, based on whatever the pid tells us
      pids[(Wheel)wheel]->run();

      analogWrite(motors[wheel].EncoderPinOut, getCalculatedOutputLevel((Wheel)wheel));
    }
    return true;
  }
  return false;
}

int VehicleMovement::getCalculatedOutputLevel(Wheel wheel) {
  // Check the direction of this wheel and the others in this row

  // If both wheels in this row are going the same direction, and this one is told to go 0, copy the other wheel instead
  // the encoder seems a little flaky
  float maxDiff = 0.1; // cant be > 10% different
  for (int wheelCount = 0; wheelCount < WHEEL_COUNT; wheelCount++) {
    if (wheel != wheelCount) {
      // if it's in the same row and going the same direction
      if (motors[wheel].WheelRow == motors[wheelCount].WheelRow && outputLevels[wheelCount] != 0 && directionForward[wheelCount] == directionForward[wheel]) {
        float otherWheelOutput = outputLevels[wheelCount];
        if (outputLevels[wheel] < otherWheelOutput && (float(otherWheelOutput - outputLevels[wheel])) / float(otherWheelOutput) > maxDiff) {
          logConsole("Diff was too big (" + String(float(abs(outputLevels[wheel] - outputLevels[wheelCount])) / float(outputLevels[wheelCount])) + "), using a calculation (" + String((outputLevels[wheelCount] * .9)) + " ) from other wheel instead of PID value");
          return int(outputLevels[wheelCount] * .9);
        }
      }
    } 
  }

  //Serial.println("Following PID directions, wheel " + String(wheel) + ": " + String(outputLevels[wheel]));
  return outputLevels[wheel];
}

// Distance traveled since last trip meter reset, not including distance since last pulse update
float VehicleMovement::getDistanceTraveled () {
  // Average the distnace of all wheels
  int wheelsIncluded = 0;
  float totalAcrossWheels = 0;
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    if (tripMeters[wheel] > 0) {
      totalAcrossWheels += tripMeters[wheel];
      wheelsIncluded++;
    }
  }

  if (wheelsIncluded == 0) {
    return 0.0;
  }
  
  return totalAcrossWheels / wheelsIncluded;
}

void VehicleMovement::resetPulses (Wheel wheel) {
  pulseCount[wheel] = 0;
  pulseCountStart[wheel] = millis();
}

void VehicleMovement::resetPulses () {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    resetPulses((Wheel)wheel);
  }
}

float VehicleMovement::getCurrentSpeed (Wheel wheel) {
  // return speed in inches per second
  float secondsPassed = (millis() - pulseCountStart[wheel]) / 1000.0;
  return getDistance(wheel) / (float)secondsPassed;
}

float VehicleMovement::getDistance (Wheel wheel) {
  // revolutions * distance travled per revolution of drive wheel
  return ((float)getRevolutions(wheel)) * motors[wheel].DriveWheelDiameter * 3.14;
}

float VehicleMovement::getRevolutions (Wheel wheel) {
  return (((float)pulseCount[wheel] / motors[wheel].PulsesPerRevolution) / motors[wheel].GearRatio);
}

void VehicleMovement::logConsole(String msg) {
  if (LOG_ENABLED) {
    Serial.println("Log: " + msg);
  }
}