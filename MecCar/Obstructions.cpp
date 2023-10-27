#include "Obstructions.h"

void Obstructions::init()
{
  Serial3.begin(9600);
  while(!Serial3);

  // bind the RPLIDAR driver to the arduino hardware serial
  lidar.begin(Serial3);

  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
}

// Returns distance to the closest obstruction within the given range
Measurement Obstructions::getClosestObstruction(float startAngle, float endAngle, float obstructionDistanceThreshold)
{
  // make sure lidar is running
  collectMeasurements(100);

  // if the end angle < start angle, it means we are crossing over zero, so do 2 loops in that case
  int scanStart = endAngle > startAngle ? startAngle : 0.0;
  Measurement closest = getClosestObstructionRangeLimited(scanStart, endAngle, obstructionDistanceThreshold);

  // If we skipped ahead to 0, we need to go back and include before 0.0
  if (scanStart != startAngle) {
    Measurement secondScanClosest = getClosestObstructionRangeLimited(startAngle, 360.0, obstructionDistanceThreshold);
    if (round(secondScanClosest.distance) > 0 && (secondScanClosest.distance < closest.distance || closest.distance == -1.0)) {
      closest.angle = secondScanClosest.angle;
      closest.distance = secondScanClosest.distance;
    }
  }

  return closest;
}

// assumes endAngle always > startAngle
Measurement Obstructions::getClosestObstructionRangeLimited(float scanStart, float scanEnd, float obstructionDistanceThreshold) {
  Measurement closest = Measurement {-1.0, -1.0};
  for (float currAngle = scanStart; currAngle <= scanEnd; currAngle += RPLIDAR_MEAS_INCREMENT) {
    int indx = getRelativeLidarIndex(currAngle);

    // See if obstruction at given angle
    if (round(lidarMeasurements[indx]) > 0 && lidarMeasurements[indx] <= obstructionDistanceThreshold) {
      // If that's closer than our already located obstruction, replace with this one
      if (closest.distance == -1.0 || lidarMeasurements[indx] < closest.distance) {
        closest.distance = lidarMeasurements[indx];
        closest.angle = currAngle;
      }
    }
  }

  return closest;
}

// Returns a pointer to the array of distances. Be careful with
// this though, as it may contain old measurements.
// You will need to use the 'getLidarIndex' method to figure
// out which index goes with which angle
float* Obstructions::getDistances()
{
  return lidarMeasurements;
}

Measurement Obstructions::getDistance(float angle, float angleTolerance, unsigned long maxAge)
{
  // make sure lidar is running
  collectMeasurements(100);

  unsigned long now = millis();
  float measuredDistance = -1.0;

  // Check the requested angle, then search half a degree in either direction until
  // we find a recent enough measurement
  for (float angleDiff = 0.0; angleDiff <= angleTolerance; angleDiff++) {
    int measIndexAbove = getRelativeLidarIndex(angle + angleDiff);
    int measIndexBelow = getRelativeLidarIndex(angle - angleDiff);
    if (lidarTimes[measIndexAbove] >= (now - maxAge) && lidarMeasurements[measIndexAbove] > 0) {
      return Measurement {angle + angleDiff, lidarMeasurements[measIndexAbove]};
    }
    if (lidarTimes[measIndexBelow] >= (now - maxAge) && lidarMeasurements[measIndexBelow] > 0) {
      return {angle - angleDiff, lidarMeasurements[measIndexBelow]};
    }
  }

  // no measurement found for that angle
  return Measurement {-1.0, -1.0};
}

// Returns a distance within the specified angle range and expected distance range
// If no matching measurements are found, returns -1
Measurement Obstructions::getDistanceInRange(float angle, float angleTolerance, float expectedDistance, float distanceTolerance, unsigned long maxAge)
{
  // make sure lidar is running
  collectMeasurements(100);

  // keep checking until we happen to catch a measurement near the given location
  unsigned long now = millis();

  float maxDist = expectedDistance + distanceTolerance;
  float minDist = expectedDistance - distanceTolerance;

  logConsole("Want measurement for angle " + String(angle) + " index " + getLidarIndex(angle) + " near " + String(expectedDistance) + " mm");

  // Check the requested angle, then search half a degree in either direction until
  // we find a recent enough measurement
  for (float angleDiff = 0.0; angleDiff <= angleTolerance; angleDiff++) {
    int measIndexAbove = getRelativeLidarIndex(angle + angleDiff);
    int measIndexBelow = getRelativeLidarIndex(angle - angleDiff);
    if (lidarTimes[measIndexAbove] >= (now - maxAge) && lidarMeasurements[measIndexAbove] >= minDist && lidarMeasurements[measIndexAbove] <= maxDist) {
      //Serial.println("Selected angle above: " + String(measIndexAbove));
      return Measurement {angle + angleDiff, lidarMeasurements[measIndexAbove]};
    }
    if (lidarTimes[measIndexBelow] >= (now - maxAge) && lidarMeasurements[measIndexBelow] >= minDist && lidarMeasurements[measIndexBelow] <= maxDist) {
      //Serial.println("Selected angle below: " + String(measIndexBelow));
      return {angle - angleDiff, lidarMeasurements[measIndexBelow]};
    }
  }

  // no measurement found for that angle
  return Measurement {-1.0, -1.0};
}

// Cleans expired measuremnts (sets them to -1)
void Obstructions::pruneMeasurements()
{
  unsigned long now = millis();
  for (int m = 0; m < RPLIDAR_NUM_MEASUREMENTS; m++) {
    if (lidarTimes[m] < (now - RPLIDAR_MEAS_EXPIRE_MILLIS)) {
      lidarMeasurements[m] = -1;
      lidarTimes[m] = -1;
    }
  }
}

void Obstructions::collectMeasurements(unsigned long timeoutMillis)
{
  unsigned long startTime = millis();
  unsigned long measurementTime = millis();

  int failCount = 0;
  int maxFails = 10;

  while ((measurementTime - startTime) < timeoutMillis) {
    if (IS_OK(lidar.waitPoint(RPLIDAR_WAIT_TIME))) {
      measurementTime = millis();

      float lidarDistance = lidar.getCurrentPoint().distance; //distance value in mm unit
      float lidarAngle    = lidar.getCurrentPoint().angle; //anglue value in degree
      bool  startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
      byte  quality  = lidar.getCurrentPoint().quality; //quality of the current measurement

      // sometimes when starting up, the lidar reports angles for > 360, which we have to ignore
      if (lidarAngle >= 0.0 && lidarAngle <= 360.0 && lidarDistance >= RPLIDAR_MIN_DISTANCE) {
        int measIndex = getLidarIndex(lidarAngle);
        lidarMeasurements[measIndex] = lidarDistance;
        lidarTimes[measIndex] = measurementTime;

        if (measIndex < 0 || measIndex > RPLIDAR_NUM_MEASUREMENTS - 1) {
          logConsole("!! Array out of bounds for angle " + String(lidarAngle) + ": " + String(measIndex));
        }
      }
    }
    else if (failCount >= maxFails) {
      logConsole("Restarting Lidar");
      startLidar();
    }
    else {
      failCount++;
    }
  }

  // clear old measurements (doesn't count toward time limit)
  pruneMeasurements();
}

int Obstructions::getLidarHeading()
{
  return RPLIDAR_HEADING;
}

int Obstructions::getNumMeasurements()
{
  return RPLIDAR_NUM_MEASUREMENTS;
}

float Obstructions::getMeasurementsIncrement()
{
  return RPLIDAR_MEAS_INCREMENT;
}

// Returns array index for measurements that corresponds most closely 
// with the given angle. This takes into account the vehicle heading
int Obstructions::getRelativeLidarIndex(float angle)
{
  float relativeAngle =  angle - RPLIDAR_HEADING;
  if (relativeAngle < 0) {
    relativeAngle += 360.0;
  }

  return round(relativeAngle / RPLIDAR_MEAS_INCREMENT);
}


// Returns array index for measurements that corresponds most closely 
// with the given angle. This does not take into account vehicle heading
int Obstructions::getLidarIndex(float angle)
{
  /*float relativeAngle =  angle - RPLIDAR_HEADING;
  if (relativeAngle < 0) {
    relativeAngle += 360.0;
  }*/

  return round(angle / RPLIDAR_MEAS_INCREMENT);
}

void Obstructions::startLidar()
{
  // Get motor running (if not already)
  analogWrite(RPLIDAR_MOTOR, 255);

  int retryCount = 0;
  while (retryCount++ < 10) {
    if (IS_OK(lidar.waitPoint(RPLIDAR_WAIT_TIME))) {
      float distance = lidar.getCurrentPoint().distance; //distance value in mm unit
      float angle    = lidar.getCurrentPoint().angle; //anglue value in degree
      bool  startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
      byte  quality  = lidar.getCurrentPoint().quality; //quality of the current measurement

    } else {
      //analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor

      // try to detect RPLIDAR...
      rplidar_response_device_info_t info;
      if (IS_OK(lidar.getDeviceInfo(info, RPLIDAR_WAIT_TIME))) {
        lidar.startScan();

        // start motor rotating at max allowed speed
        //analogWrite(RPLIDAR_MOTOR, 255);
        //delay(1000);
        logConsole("Lidar OK");
        retryCount = 10;
      }
      else {
        logConsole("Lidar not detected");
        delay(1000);
      }
    }
  }

}

void Obstructions::stopLidar()
{
  analogWrite(RPLIDAR_MOTOR, 0);
}

void Obstructions::logConsole(String msg) {
  if (LOG_ENABLED) {
    Serial.println("Log:" + msg);
  }
}

