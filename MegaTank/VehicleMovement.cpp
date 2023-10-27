#include "VehicleMovement.h"

VehicleMovement::VehicleMovement () {
  motors[LEFT_REAR].EncoderAIn = 2;
  motors[LEFT_REAR].EncoderPinOut = 4;//4
  motors[LEFT_REAR].OutputPin1 = 7; //6
  motors[LEFT_REAR].OutputPin2 = 8; //5

  motors[LEFT_REAR].GearRatio = 150;
  motors[LEFT_REAR].TrackLength = 23.0;
  motors[LEFT_REAR].DriveWheelDiameter = 6.0;
  motors[LEFT_REAR].PulsesPerRevolution = 2;
  motors[LEFT_REAR].OutputMin = 150;
  motors[LEFT_REAR].OutputMax = 255;
  motors[LEFT_REAR].WheelRow = 1; // secnd row
  motors[LEFT_REAR].WheelSide = SIDE_LEFT; // left

  motors[RIGHT_REAR].EncoderAIn = 3;
  motors[RIGHT_REAR].EncoderPinOut = 9; //9
  motors[RIGHT_REAR].OutputPin1 = 6; //7
  motors[RIGHT_REAR].OutputPin2 = 5; //8

  motors[RIGHT_REAR].GearRatio = 150;
  motors[RIGHT_REAR].TrackLength = 23.0;
  motors[RIGHT_REAR].DriveWheelDiameter = 6.0;
  motors[RIGHT_REAR].PulsesPerRevolution = 2;
  motors[RIGHT_REAR].OutputMin = 150;
  motors[RIGHT_REAR].OutputMax = 255;
  motors[RIGHT_REAR].WheelRow = 1; // second row
  motors[RIGHT_REAR].WheelSide = SIDE_RIGHT; // right

  motors[LEFT_FRONT].EncoderAIn = 18;
  motors[LEFT_FRONT].EncoderPinOut = 33; // was 23
  motors[LEFT_FRONT].OutputPin1 = 31;//27; // was 27 : 31
  motors[LEFT_FRONT].OutputPin2 = 29;//25; // was 25 : 29

  motors[LEFT_FRONT].GearRatio = 150;
  motors[LEFT_FRONT].TrackLength = 23.0;
  motors[LEFT_FRONT].DriveWheelDiameter = 6.0;
  motors[LEFT_FRONT].PulsesPerRevolution = 2;
  motors[LEFT_FRONT].OutputMin = 150;
  motors[LEFT_FRONT].OutputMax = 255;
  motors[LEFT_FRONT].WheelRow = 0; // first row
  motors[LEFT_FRONT].WheelSide = SIDE_RIGHT; // left

  motors[RIGHT_FRONT].EncoderAIn = 19;
  motors[RIGHT_FRONT].EncoderPinOut = 23;  // was 33
  motors[RIGHT_FRONT].OutputPin1 = 25;//29;//29; // was 29
  motors[RIGHT_FRONT].OutputPin2 = 27;//31; // was 31

  motors[RIGHT_FRONT].GearRatio = 150;
  motors[RIGHT_FRONT].TrackLength = 23.0;
  motors[RIGHT_FRONT].DriveWheelDiameter = 6.0;
  motors[RIGHT_FRONT].PulsesPerRevolution = 2;
  motors[RIGHT_FRONT].OutputMin = 150;
  motors[RIGHT_FRONT].OutputMax = 255;
  motors[RIGHT_FRONT].WheelRow = 0; // first row
  motors[RIGHT_FRONT].WheelSide = SIDE_LEFT; // right



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
  // Encoders
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    pinMode(motors[wheel].EncoderAIn , INPUT_PULLUP);
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

void VehicleMovement::setDirectionSpeed (Wheel wheel, bool forward, float inchesPerSecond) {
  targetSpeeds[wheel] = inchesPerSecond;

  // Pull the setting from the PID
  pids[wheel]->run();

	digitalWrite(motors[wheel].OutputPin1, forward ? HIGH : LOW);
	digitalWrite(motors[wheel].OutputPin2, forward ? LOW : HIGH);

  //Serial.println("  wheel " + String(wheel) + " getting output of " + String(getCalculatedOutputLevel(wheel)));
  analogWrite(motors[wheel].EncoderPinOut, getCalculatedOutputLevel(wheel));
  //analogWrite(motors[wheel].EncoderPinOut, 255);  
  directionForward[wheel] = forward;
}

void VehicleMovement::setDirectionSpeed (bool forward, float inchesPerSecond) {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    setDirectionSpeed(wheel, forward, inchesPerSecond);
  }
}

void VehicleMovement::rotate (float degrees) {
  stop();

  unsigned long rotationTime = abs(int(degrees / 90)) * 1000; // assumes rotates 90 degrees per second
  bool leftRotationForward = degrees > 0; // left side will go forward if rotating right
  bool rightRotationForward = degrees < 0; // right side will go forward if rotating left
  unsigned long rotationSpeed = motors[0].OutputMax; // pick a random motor, its max will be our output to all wheels

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
      currentSpeeds[wheel] = getCurrentSpeed(wheel);

      // bump the trip meter
      tripMeters[wheel] += getDistance(wheel);//(directionForward[wheel] ? 1.0 : -1.0) * getDistance(wheel);

      // reset teh pulse count
      resetPulses(wheel);
      Serial.println("----> Wheel " + String(wheel) + " Speed: " + String(currentSpeeds[wheel]));

      // update, based on whatever the pid tells us
      pids[wheel]->run();

      analogWrite(motors[wheel].EncoderPinOut, getCalculatedOutputLevel(wheel));
    }
    return true;
  }
  return false;
}

int VehicleMovement::getCalculatedOutputLevel(Wheel wheel) {
  //return 255;

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
          Serial.println("Diff was too big (" + String(float(abs(outputLevels[wheel] - outputLevels[wheelCount])) / float(outputLevels[wheelCount])) + "), using a calculation (" + String((outputLevels[wheelCount] * .9)) + " ) from other wheel instead of PID value");
          return int(outputLevels[wheelCount] * .9);
        }
      }
    } 
  }

  Serial.println("Following PID directions, wheel " + String(wheel) + ": " + String(outputLevels[wheel]));
  return outputLevels[wheel];
}

// Distance traveled since last trip meter reset, not including distance since last pulse update
float VehicleMovement::getDistanceTraveled () {
  // Average the distnace of all wheels
  float totalAcrossWheels = 0;
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    totalAcrossWheels += tripMeters[wheel];
  }
  return totalAcrossWheels / WHEEL_COUNT;
}

void VehicleMovement::resetPulses (Wheel wheel) {
  pulseCount[wheel] = 0;
  pulseCountStart[wheel] = millis();
}

void VehicleMovement::resetPulses () {
  for (int wheel = 0; wheel < WHEEL_COUNT; wheel++) {
    resetPulses(wheel);
  }
}

float VehicleMovement::getCurrentSpeed (Wheel wheel) {
  // return speed in inches per second
  float secondsPassed = (millis() - pulseCountStart[wheel]) / 1000.0;
  return getDistance(wheel) / secondsPassed;
}

float VehicleMovement::getDistance (Wheel wheel) {
  // revolutions * distance travled per revolution of drive wheel
  return getRevolutions(wheel) * (motors[wheel].TrackLength / motors[wheel].DriveWheelDiameter);
}

int VehicleMovement::getRevolutions (Wheel wheel) {
  return ((pulseCount[wheel] / motors[wheel].PulsesPerRevolution) / motors[wheel].GearRatio);
}
