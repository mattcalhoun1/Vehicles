#include <Arduino.h>
#include "src/RPLidarDriver//RPLidar.h"

// 39-45 odd in mega
// 7,8 12,13 on uno
#define TRIG_PIN_LEFT 40      // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_LEFT 42      // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIG_PIN_RIGHT 36      
#define ECHO_PIN_RIGHT 38      

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// You need to create an driver instance

#define RPLIDAR_MOTOR 11 // The PWM pin for control the speed of RPLIDAR's motor.
                        // This pin should connected with the RPLIDAR's MOTOCTRL signal
#define RPLIDAR_HEADING 90 // degrees relative to the mount (vehicle)

#define RPLIDAR_NUM_MEASUREMENTS 721 // if this is changed, calculations that assume .5 increments may need changed
#define RPLIDAR_MEAS_INCREMENT 0.5 // if # measurements above is changed, this should also be changed to match up
#define RPLIDAR_MEAS_EXPIRE_MILLIS 5000 // how long (minimum) a measurement can exist before being pruned.

struct Measurement {
  float angle;
  float distance;
};

class Obstructions
{
public:
  void init ();
  int getDistanceBumperLeft();
  int getDistanceBumperRight();
  void startLidar ();
  void stopLidar ();
  Measurement getDistance(float angle, float angleTolerance, unsigned long maxAge);
  Measurement getDistanceInRange (float angle, float angleTolerance, float expectedDistance, float distanceTolerance, unsigned long maxAge);
  void collectMeasurements(unsigned long timeoutMillis);
  Measurement getClosestObstruction(float startAngle, float endAngle, float obstructionDistanceThreshold);
  int getLidarHeading ();
  float* getDistances();
  int getNumMeasurements();
  float getMeasurementsIncrement();

private:
  RPLidar lidar;
  float lidarMeasurements[RPLIDAR_NUM_MEASUREMENTS];
  unsigned long lidarTimes[RPLIDAR_NUM_MEASUREMENTS];
  void pruneMeasurements();
  Measurement getClosestObstructionRangeLimited(float scanStart, float scanEnd, float obstructionDistanceThreshold);
  int getLidarIndex(float angle);
  int getRelativeLidarIndex(float angle);
};
