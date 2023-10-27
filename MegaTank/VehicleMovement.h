#include "MotorInfo.h"
#include <Arduino.h>
#include <AutoPID.h>

#define DRIVE_READ_DELAY 100 //can only read digital temp sensor every ~750ms

#define DRIVE_KP 0.05 //.12
#define DRIVE_KI 25 //.0003
#define DRIVE_KD 0.001//0

#define WHEEL_COUNT 4
enum Wheel{
  LEFT_REAR = 0,
  RIGHT_REAR = 1,
  LEFT_FRONT = 2,
  RIGHT_FRONT = 3
  //LEFT_FRONT = 3,
  //RIGHT_FRONT = 4
};

enum WheelSide{
  SIDE_LEFT = 0,
  SIDE_RIGHT = 1
};


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
    int getRevolutions (Wheel wheel);
    int getWheelCount ();
    void resetTripMeter ();
    void rotate(float degrees); // -90 = left, +90 = right, + or - 180 turn around, etc

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
};


