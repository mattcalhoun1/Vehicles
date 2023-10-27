#include <Wire.h>
//#include <PCA9685.h>

#include <Adafruit_PWMServoDriver.h>

#define VEHICLE_SERVOS_PCA_ADDRESS 0x40

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

class VehicleServos {
  public:
    VehicleServos ();
    void setCameraRotation(int angle, int cameraNum);
    void setCameraTilt(int angle, int cameraNum);
    void setVehicleRelativeRotation (int angle, int cameraNum);
    int getVehicleRelativeRotation (int cameraNum);
    int getCameraRotation(int cameraNum);
    int getCameraTilt(int cameraNum);
    int getCameraFor (int rotation, int tilt);
    void init (int startingRotations[], int startingTilts[]);
    int getNumCameras ();
    int getMinVehicleRelativeRotation (int cameraNum);
    int getMaxVehicleRelativeRotation (int cameraNum);
    int getMinTilt (int cameraNum);
    int getMaxTilt (int cameraNum);

  private:
    //PCA9685 pca;
    Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver();
    int cameraTilts[NUM_CAMERAS];
    int cameraRotations[NUM_CAMERAS];

    int channelsTilt[NUM_CAMERAS];
    int channelsRotation[NUM_CAMERAS];
    int vehicleFore[NUM_CAMERAS];
    int vehicleAft[NUM_CAMERAS];
    int vehicleRelativeMax[NUM_CAMERAS];
    int vehicleRelativeMin[NUM_CAMERAS];
    int tiltMax[NUM_CAMERAS];
    int tiltMin[NUM_CAMERAS];

    CameraMountPosition mountPositions[NUM_CAMERAS];
    int getPulseWidth(int angle, int servoMinAngle, int servoMaxAngle);
};