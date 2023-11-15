#include "MotorInfo.h"
#include <Arduino.h>
#include <AutoPID.h>
#include "Globals.h"

#ifndef VEHICLE_MOVEMENT_H
#define VEHICLE_MOVEMENT_H
#define DRIVE_READ_DELAY 100 // how often to check sensors while driving

#define DRIVE_KP 0.05 //.12
#define DRIVE_KI 25 //.0003
#define DRIVE_KD 0.001//0

#define MIN_MOTOR_OUTPUT 130
#define MAX_MOTOR_OUTPUT 190
#define STRAFE_MOTOR_OUTPUT 255

#define WHEEL_COUNT 4
enum Wheel{
  LEFT_REAR = 0,
  RIGHT_REAR = 1,
  LEFT_FRONT = 2,
  RIGHT_FRONT = 3
};

enum StrafeDirection {
  LEFT_FORWARD = 0,
  LEFT_LEFT = 1,
  LEFT_BACKWARD = 2,
  RIGHT_FORWARD = 3,
  RIGHT_RIGHT = 4,
  RIGHT_BACKWARD = 5,
  UNKNOWN_DIRECTION = 6
};

enum WheelSide{
  SIDE_LEFT = 0,
  SIDE_RIGHT = 1
};

/**
 * Enables and measures vehicle motion (driving) 
 */
class VehicleMovement {
  public:
    VehicleMovement ();
    ~VehicleMovement ();
    void setDirectionSpeed (Wheel wheel, bool forward, float inchesPerSecond);
    void setDirectionSpeed (bool forward, float inchesPerSecond);
    void stop ();
    void resetPulses (Wheel wheel);
    void resetPulses ();
    float getCurrentSpeed (Wheel wheel);
    float getDistance (Wheel wheel);
    float getDistanceTraveled ();
    float getRevolutions (Wheel wheel);
    int getWheelCount ();
    void resetTripMeter ();
    void rotate(float degrees); // -90 = left, +90 = right, + or - 180 turn around, etc
    void strafe (StrafeDirection direction, unsigned long durationMillis);
    bool supportsStrafe () {return true;}

    // Interrupt-related
    int getEncoderAPin (Wheel wheel); // for reading speed / attaching interrupt
    void interrupt (Wheel wheel);
    bool updateSpeeds ();
  private:
    MotorInfo motors[WHEEL_COUNT];
    unsigned long lastSpeedUpdate = 0; //tracks clock time of last update
    float tripMeters[WHEEL_COUNT];
    unsigned long pulseCount[WHEEL_COUNT];
    unsigned long pulseCountStart[WHEEL_COUNT];
    bool directionForward[WHEEL_COUNT];
    double currentSpeeds[WHEEL_COUNT];
    double targetSpeeds[WHEEL_COUNT];
    double outputLevels[WHEEL_COUNT];
    AutoPID * pids[WHEEL_COUNT];
    int getCalculatedOutputLevel(Wheel wheel);
    void logConsole(String msg);
};
#endif