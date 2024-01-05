#include "CameraServos.h"

CameraServos::CameraServos () {
}

void CameraServos::init (int startingRotations[], int startingTilts[]) {
  WIRE.begin();

  //servoDriver.setOscillatorFrequency(27000000);
  servoDriver.begin();

  //servoDriver.setOscillatorFrequency(27000000);
  servoDriver.setPWMFreq(200);  // Analog servos run at ~50 Hz updates

  // front left
  channelsRotation[0] = 0;
  channelsTilt[0] = 1;
  //vehicleFore[0] = 26;
  //vehicleAft[0] = 182;
  distanceFromCenter[0] = 220;//mm
  frameRelativeMax[0] = 160;
  frameRelativeMin[0] = 0;
  tiltMin[0] = 20;
  tiltMax[0] = 140;
  tiltAdjust[0] = -1;// 9;
  rotationAdjust[0] = 6;
  mountPositions[0] = CAM_MOUNT_LEFT;

  // front right
  channelsRotation[1] = 2;
  channelsTilt[1] = 3;
  distanceFromCenter[1] = 220;//mm
  //vehicleFore[1] = 176;
  //vehicleAft[1] = 27;
  frameRelativeMax[1] = 180;
  frameRelativeMin[1] = 20;
  tiltMin[1] = 20;
  tiltMax[1] = 140;
  tiltAdjust[1] = 4;//-11;
  rotationAdjust[1] = -3;
  mountPositions[1] = CAM_MOUNT_RIGHT;

  // set the default angles
  for (int camNum = 0; camNum < NUM_CAMERAS; camNum++) {
      setCameraTilt(startingTilts[camNum], camNum);
      setFrameRelativeRotation(startingRotations[camNum], camNum);
  }
  delay(2000); // give servos time to get there
}

int CameraServos::getNumCameras() {
  return NUM_CAMERAS;
}

int CameraServos::getCameraFor(int rotation, int tilt) {
  for (int camCount = 0; camCount < NUM_CAMERAS; camCount++) {
    if (frameRelativeMax[camCount] >= rotation && frameRelativeMin[camCount] <= rotation) {
      if (tiltMax[camCount] >= tilt && tiltMin[camCount] <= tilt) {
        return camCount;
      }
    }
  } 

  // no cameras match 
  return -1;
}

int CameraServos::getMinFrameRelativeRotation (int cameraNum) {
 return frameRelativeMin[cameraNum];
}

int CameraServos::getMaxFrameRelativeRotation (int cameraNum) {
  return frameRelativeMax[cameraNum];
}

int CameraServos::getMinTilt (int cameraNum) {
  return tiltMin[cameraNum];
}

int CameraServos::getMaxTilt (int cameraNum) {
  return tiltMax[cameraNum];
}

CameraMountPosition CameraServos::getMountPosition(int cameraNum) {
  return mountPositions[cameraNum];
}

void CameraServos::setFrameRelativeRotation (int angle, int cameraNum) {
  if (angle > frameRelativeMax[cameraNum] || angle < frameRelativeMin[cameraNum]) {
    if (LOG_ENABLED) {
      Serial.println("Log: ERROR - Out of range rotation angle " + String(angle) + " received for camera " + String(cameraNum));
    }
    return;
  }

  // todo: calculate relative angle from center of frame, rather than simply passing the angle through
  int nextAngle = 0;  
  /*if (mountPositions[cameraNum] == CAM_MOUNT_RIGHT) {
    nextAngle = map(angle, 0, 180, vehicleFore[cameraNum], vehicleAft[cameraNum]);
  }
  else if (mountPositions[cameraNum] == CAM_MOUNT_LEFT) {
    nextAngle = map(angle, -180, 0, vehicleAft[cameraNum], vehicleFore[cameraNum]);
  }*/

  nextAngle = angle;

  setCameraRotation(nextAngle, cameraNum);
}

int CameraServos::getFrameRelativeRotation (int cameraNum) {

  // todo: do a calculation from the cetner of the frame
  // for now, assume the camera is sitting at the cetner of the frame
  /*if (mountPositions[cameraNum] == CAM_MOUNT_RIGHT) {
    return map(cameraRotations[cameraNum], vehicleFore[cameraNum], vehicleAft[cameraNum], 0, 180);
  }

  return map(cameraRotations[cameraNum], vehicleAft[cameraNum], vehicleFore[cameraNum], -180, 0);
  */
  return cameraRotations[cameraNum];
}

void CameraServos::setCameraRotation(int angle, int cameraNum) {
  int newPulseWidth = getPulseWidth(angle + rotationAdjust[cameraNum], CAM_SERVO_MIN_ROTATION, CAM_SERVO_MAX_ROTATION);
  int result = servoDriver.setPWM(channelsRotation[cameraNum], 0, newPulseWidth);
  if (LOG_ENABLED && result != 0) {
    Serial.println("Log:Received nonzero result: " + String(result) + " from PWM set on pca motor: " + String(channelsRotation[cameraNum]));
  }

  cameraRotations[cameraNum] = angle;
}

void CameraServos::setCameraTilt(int angle, int cameraNum) {
  int newPulseWidth = getPulseWidth(angle + tiltAdjust[cameraNum], CAM_SERVO_MIN_TILT, CAM_SERVO_MAX_TILT);
  int result = servoDriver.setPWM(channelsTilt[cameraNum], 0, newPulseWidth);
  if (LOG_ENABLED && result != 0) {
    Serial.println("Log:Received nonzero result: " + String(result) + " from PWM set on pca motor: " + String(channelsTilt[cameraNum]));
  }

  cameraTilts[cameraNum] = angle;
}

int CameraServos::getCameraRotation(int cameraNum) {
  return cameraRotations[cameraNum];
}

int CameraServos::getCameraTilt(int cameraNum) {
  return cameraTilts[cameraNum];  
}

int CameraServos::getPulseWidth(int angle, int servoMinAngle, int servoMaxAngle) {
  int newPulseWidth = map(angle, servoMinAngle, servoMaxAngle, YB_SERVO_MIN_PULSE, YB_SERVO_MAX_PULSE);
  return newPulseWidth;
}
