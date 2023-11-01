#include "VehicleServos.h"

VehicleServos::VehicleServos () {
}

void VehicleServos::init (int startingRotations[], int startingTilts[]) {
  WIRE.begin();

  //servoDriver.setOscillatorFrequency(27000000);
  servoDriver.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  //servoDriver.setOscillatorFrequency(27000000);
  servoDriver.setPWMFreq(200);  // Analog servos run at ~50 Hz updates

  // front left
  channelsRotation[0] = 0;
  channelsTilt[0] = 1;
  vehicleFore[0] = 25;
  vehicleAft[0] = 179;
  vehicleRelativeMax[0] = 0;
  vehicleRelativeMin[0] = -170;
  tiltMin[0] = 20;
  tiltMax[0] = 140;
  tiltAdjust[0] = 9;
  mountPositions[0] = CAM_MOUNT_LEFT;

  // front right
  channelsRotation[1] = 2;
  channelsTilt[1] = 3;
  vehicleFore[1] = 175;
  vehicleAft[1] = 25;
  vehicleRelativeMax[1] = 170;
  vehicleRelativeMin[1] = 0;
  tiltMin[1] = 20;
  tiltMax[1] = 140;
  tiltAdjust[1] = -11;
  mountPositions[1] = CAM_MOUNT_RIGHT;

  // set the default angles
  for (int camNum = 0; camNum < NUM_CAMERAS; camNum++) {
      setCameraTilt(startingTilts[camNum], camNum);
      setVehicleRelativeRotation(startingRotations[camNum], camNum);
  }
  delay(2000); // give servos time to get there
}

int VehicleServos::getNumCameras() {
  return NUM_CAMERAS;
}

int VehicleServos::getCameraFor(int rotation, int tilt) {
  for (int camCount = 0; camCount < NUM_CAMERAS; camCount++) {
    if (vehicleRelativeMax[camCount] >= rotation && vehicleRelativeMin[camCount] <= rotation) {
      if (tiltMax[camCount] >= tilt && tiltMin[camCount] <= tilt) {
        return camCount;
      }
    }
  } 

  // no cameras match 
  return -1;
}

int VehicleServos::getMinVehicleRelativeRotation (int cameraNum) {
 return vehicleRelativeMin[cameraNum];
}

int VehicleServos::getMaxVehicleRelativeRotation (int cameraNum) {
  return vehicleRelativeMax[cameraNum];
}

int VehicleServos::getMinTilt (int cameraNum) {
  return tiltMin[cameraNum];
}

int VehicleServos::getMaxTilt (int cameraNum) {
  return tiltMax[cameraNum];
}

CameraMountPosition VehicleServos::getMountPosition(int cameraNum) {
  return mountPositions[cameraNum];
}

void VehicleServos::setVehicleRelativeRotation (int angle, int cameraNum) {
  if (angle > vehicleRelativeMax[cameraNum] || angle < vehicleRelativeMin[cameraNum]) {
    if (LOG_ENABLED) {
      Serial.println("Log: ERROR - Out of range rotation angle " + String(angle) + " received for camera " + String(cameraNum));
    }
    return;
  }

  int nextAngle = 0;  
  if (mountPositions[cameraNum] == CAM_MOUNT_RIGHT) {
    nextAngle = map(angle, 0, 180, vehicleFore[cameraNum], vehicleAft[cameraNum]);
  }
  else if (mountPositions[cameraNum] == CAM_MOUNT_LEFT) {
    nextAngle = map(angle, -180, 0, vehicleAft[cameraNum], vehicleFore[cameraNum]);
  }

  setCameraRotation(nextAngle, cameraNum);
}

int VehicleServos::getVehicleRelativeRotation (int cameraNum) {
  if (mountPositions[cameraNum] == CAM_MOUNT_RIGHT) {
    return map(cameraRotations[cameraNum], vehicleFore[cameraNum], vehicleAft[cameraNum], 0, 180);
  }

  return map(cameraRotations[cameraNum], vehicleAft[cameraNum], vehicleFore[cameraNum], -180, 0);
}

void VehicleServos::setCameraRotation(int angle, int cameraNum) {
  int newPulseWidth = getPulseWidth(angle, CAM_SERVO_MIN_ROTATION, CAM_SERVO_MAX_ROTATION);
  int result = servoDriver.setPWM(channelsRotation[cameraNum], 0, newPulseWidth);
  if (LOG_ENABLED && result != 0) {
    Serial.println("Log:Received nonzero result: " + String(result) + " from PWM set on pca motor: " + String(channelsRotation[cameraNum]));
  }

  cameraRotations[cameraNum] = angle;
}

void VehicleServos::setCameraTilt(int angle, int cameraNum) {
  int newPulseWidth = getPulseWidth(angle + tiltAdjust[cameraNum], CAM_SERVO_MIN_TILT, CAM_SERVO_MAX_TILT);
  int result = servoDriver.setPWM(channelsTilt[cameraNum], 0, newPulseWidth);
  if (LOG_ENABLED && result != 0) {
    Serial.println("Log:Received nonzero result: " + String(result) + " from PWM set on pca motor: " + String(channelsTilt[cameraNum]));
  }

  cameraTilts[cameraNum] = angle;
}

int VehicleServos::getCameraRotation(int cameraNum) {
  return cameraRotations[cameraNum];
}

int VehicleServos::getCameraTilt(int cameraNum) {
  return cameraTilts[cameraNum];  
}

int VehicleServos::getPulseWidth(int angle, int servoMinAngle, int servoMaxAngle) {
  int newPulseWidth = map(angle, servoMinAngle, servoMaxAngle, YB_SERVO_MIN_PULSE, YB_SERVO_MAX_PULSE);
  return newPulseWidth;
}
