#include <Arduino.h>
#include "src/RPLidarDriver//RPLidar.h"
#include "Globals.h"

#ifndef OBSTRUCTIONS_H
#define OBSTRUCTIONS_H

#define RPLIDAR_MOTOR 11 // The PWM pin for control the speed of RPLIDAR's motor.
                        // This pin should connected with the RPLIDAR's MOTOCTRL signal
#define RPLIDAR_HEADING 4 // degrees relative to the mount (vehicle)

#define RPLIDAR_NUM_MEASUREMENTS 1461 // if this is changed, calculations that assume .5 increments may need changed
#define RPLIDAR_MEAS_INCREMENT 0.25 // if # measurements above is changed, this should also be changed to match up

#define RPLIDAR_MEAS_EXPIRE_MILLIS 6000 // how long (minimum) a measurement can exist before being pruned.

#define RPLIDAR_MIN_DISTANCE 200.0 // ignore any measurements less than this
#define RPLIDAR_WAIT_TIME 100

struct Measurement {
  float angle;
  float distance;
};

/**
 * Receives measurements streaming from lidar, stores them in an array,
 * pruning old measurements and replacing existing ones as new ones arrive.
 */
class Obstructions
{
public:
  void init ();
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
  void logConsole(String msg);
};

#endif