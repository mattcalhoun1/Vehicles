#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Globals.h"

#define WIRE Wire

#ifndef CAMERA_SERVOS_H
#define CAMERA_SERVOS_H

#define CAMERA_SERVOS_PCA_ADDRESS 0x40 // 0x40 or 0x70

#define YB_SERVO_MIN_PULSE 500
#define YB_SERVO_MAX_PULSE 2500

#define CAM_SERVO_MIN_ROTATION 0
#define CAM_SERVO_MAX_ROTATION 270

#define CAM_SERVO_MIN_TILT 0
#define CAM_SERVO_MAX_TILT 180

enum CameraMountPosition{
  CAM_MOUNT_LEFT = 0,
  CAM_MOUNT_RIGHT = 1
};

#define NUM_CAMERAS 2

class CameraServos {
  public:
    CameraServos ();
    void setCameraRotation(int angle, int cameraNum);
    void setCameraTilt(int angle, int cameraNum);
    void setFrameRelativeRotation (int angle, int cameraNum);
    int getFrameRelativeRotation (int cameraNum);
    int getCameraRotation(int cameraNum);
    int getCameraTilt(int cameraNum);
    int getCameraFor (int rotation, int tilt);
    void init (int startingRotations[], int startingTilts[]);
    int getNumCameras ();
    int getMinFrameRelativeRotation (int cameraNum);
    int getMaxFrameRelativeRotation (int cameraNum);
    int getMinTilt (int cameraNum);
    int getMaxTilt (int cameraNum);
    CameraMountPosition getMountPosition (int cameraNum);

  private:
    Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(CAMERA_SERVOS_PCA_ADDRESS, Wire);
    int cameraTilts[NUM_CAMERAS];
    int cameraRotations[NUM_CAMERAS];

    int channelsTilt[NUM_CAMERAS];
    int channelsRotation[NUM_CAMERAS];
    //int vehicleFore[NUM_CAMERAS];
    //int vehicleAft[NUM_CAMERAS];
    int distanceFromCenter[NUM_CAMERAS];
    int frameRelativeMax[NUM_CAMERAS];
    int frameRelativeMin[NUM_CAMERAS];
    int tiltMax[NUM_CAMERAS];
    int tiltMin[NUM_CAMERAS];
    int tiltAdjust[NUM_CAMERAS];

    CameraMountPosition mountPositions[NUM_CAMERAS];
    int getPulseWidth(int angle, int servoMinAngle, int servoMaxAngle);
};

#endif