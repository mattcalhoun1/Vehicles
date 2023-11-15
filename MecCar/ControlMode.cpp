#include "ControlMode.h"

ControlMode::ControlMode(Lookout* _lookout, Obstructions* _obstructions, VehicleMovement* _movement, VehicleServos* _servos, Commander* _commander) {
  lookout = _lookout;
  obstructions = _obstructions;
  movement = _movement;
  servos = _servos;
  commander = _commander;
}

/*******************
 * Sensing         *
 ******************/
bool ControlMode::isObstructed () {
  bool usLeftObstructed = false, usRightObstructed = false;
  int leftBumperDist = 0, rightBumperDist = 0;
  if (lookout->bumperSensorsEnabled()) {
    lookout->refreshDistances();
    leftBumperDist = lookout->getDistanceBumperLeft();
    rightBumperDist = lookout->getDistanceBumperRight();
    usLeftObstructed = leftBumperDist > 0 && leftBumperDist <= US_OBSTRUCTION_THRESHOLD;
    usRightObstructed = rightBumperDist > 0 && rightBumperDist <= US_OBSTRUCTION_THRESHOLD;
  }

  if (usLeftObstructed) {
    logConsole(STR_STATUS_OBSTRUCTION_US);
    lookout->showObstruction(STR_FL, leftBumperDist);
    return true;
  }
  if (usRightObstructed) {
    logConsole(STR_STATUS_OBSTRUCTION_US);
    lookout->showObstruction(STR_FR, rightBumperDist);
    return true;
  }

  Measurement closestLidarObst = obstructions->getClosestObstruction(360.0 - 25.0, 25.0, LD_OBSTRUCTION_THRESHOLD, ACTIVE_MOVEMENT_CYCLE_TIME);
  bool lidarObstructed = closestLidarObst.distance > -1.0 && closestLidarObst.distance <= LD_OBSTRUCTION_THRESHOLD;

  if (lidarObstructed) {
    lookout->showObstruction(String(closestLidarObst.angle), (int)closestLidarObst.distance);
  }

  return usLeftObstructed || usLeftObstructed || lidarObstructed;
}
bool ControlMode::isRearObstructed () {
  bool usLeftObstructed = false, usRightObstructed = false;
  int leftBumperDist = 0, rightBumperDist = 0;
  if (lookout->bumperSensorsEnabled()) {
    lookout->refreshDistances();
    leftBumperDist = lookout->getDistanceRearBumperLeft();
    rightBumperDist = lookout->getDistanceRearBumperRight();
    usLeftObstructed = leftBumperDist > 0 && leftBumperDist <= US_OBSTRUCTION_THRESHOLD;
    usRightObstructed = rightBumperDist > 0 && rightBumperDist <= US_OBSTRUCTION_THRESHOLD;
  }

  if (usLeftObstructed) {
    lookout->showObstruction(STR_RL, leftBumperDist);
    logConsole(STR_STATUS_OBSTRUCTION_US);
    return true;
  }
  if (usRightObstructed) {
    lookout->showObstruction(STR_RR, rightBumperDist);
    logConsole(STR_STATUS_OBSTRUCTION_US);
    return true;
  }

  Measurement closestLidarObst = obstructions->getClosestObstruction(180.0 - 25.0, 180.0 + 25.0, LD_OBSTRUCTION_THRESHOLD, ACTIVE_MOVEMENT_CYCLE_TIME);
  bool lidarObstructed = closestLidarObst.distance > -1.0 && closestLidarObst.distance <= LD_OBSTRUCTION_THRESHOLD;

  if (lidarObstructed) {
    lookout->showObstruction(String(closestLidarObst.angle), (int)closestLidarObst.distance);
    return true;
  }

  return false;
}

bool ControlMode::isStrafeObstructed (StrafeDirection direction) {
  // Angle to check depends on direction
  float scanStart = 0.0, scanEnd = 0.0, scanThreshold = LD_OBSTRUCTION_THRESHOLD;
  float sideAngleWidthHalf = 10.0; // how many degrees either side of x axis to check for obstruction

  switch (direction) {
    case LEFT_FORWARD:
      scanStart = 270.0;
      scanEnd = 360.0;
      break;
    case LEFT_LEFT:
      scanStart = 270.0 - sideAngleWidthHalf;
      scanEnd = 270.0 + sideAngleWidthHalf;
      break;
    case LEFT_BACKWARD:
      scanStart = 180.0;
      scanEnd = 270.0;
      break;
    case RIGHT_FORWARD:
      scanStart = 0.0;
      scanEnd = 90.0;
      break;
    case RIGHT_RIGHT:
      scanStart = 90.0 - sideAngleWidthHalf;
      scanEnd = 90.0 + sideAngleWidthHalf;
      break;
    case RIGHT_BACKWARD:
      scanStart = 90.0;
      scanEnd = 180.0;
      break;
  }  

  Measurement closestLidarObst = obstructions->getClosestObstruction(scanStart, scanEnd, US_OBSTRUCTION_THRESHOLD, ACTIVE_MOVEMENT_CYCLE_TIME);
  bool lidarObstructed = closestLidarObst.distance > -1.0;

  if (lidarObstructed) {
    lookout->showObstruction(String(closestLidarObst.angle), (int)closestLidarObst.distance);
  }

  return lidarObstructed;
  
}



/*****************
 * Movement      *
 *****************/
bool ControlMode::strafe (StrafeDirection direction, int duration) {
  long startTime = millis();
  short retryCount = 0, maxRetries = 2;
  unsigned long strafeTimeLeft = duration; // strafe countdown clock
  while (strafeTimeLeft > 0) {
    if(isStrafeObstructed(direction)) {
      lookout->showObstruction(nameForStrafeDirection(direction), 0);
      retryCount++;
      if (retryCount >= maxRetries) {
        return false;
      }
      lookout->showStatus(STR_STATUS_MEASURING);
      sleepOrBackground(LD_FULL_CYCLE_TIME);
    }

    long thisCycleDuration = (strafeTimeLeft < ACTIVE_STRAFE_CYCLE_TIME) ? strafeTimeLeft : ACTIVE_STRAFE_CYCLE_TIME;
    movement->strafe(direction, thisCycleDuration);
    
    // remove this time from strafe clock. if there's more left, remeasure
    strafeTimeLeft -= thisCycleDuration;
    if (strafeTimeLeft > 0) {
      lookout->showStatus(STR_STATUS_MEASURING);
      sleepOrBackground(LD_FULL_CYCLE_TIME);
    }
  }

  return true;
}

bool ControlMode::driveForward (int distance, float speed) {
  driveForward (distance, speed, strafeEnabled());
}

bool ControlMode::driveForward (int distance, float speed, bool strafeAllowed) {
  // needs a timeout
  lookout->showStatus(STR_STATUS_GO_FORWARD);
  movement->resetPulses();
  movement->resetTripMeter();

  movement->setDirectionSpeed(true, speed);
  bool allTheWay = true;
  short strafeCount = 0, maxStrafes = 2;
  unsigned long strafeDuration = 2000; // 2 sec
  while (movement->getDistanceTraveled() < distance) {
    sleepOrBackground(ACTIVE_MOVEMENT_CYCLE_TIME);
    movement->updateSpeeds();

    // Check for obstuctions
    if (isObstructed()) {
      movement->stop();

      if (!strafeAllowed) {
        return false;
      }

      lookout->showStatus(STR_STATUS_MEASURING);

      // Allow lidar to run a cycle and check again
      sleepOrBackground(LD_FULL_CYCLE_TIME);

      if (isObstructed()) {
        // Try strafing one direction or the other, if possible
        if (strafeAllowed && movement->supportsStrafe() && strafeCount < maxStrafes) {
          if (!isStrafeObstructed(LEFT_LEFT)) {
            strafeCount++;
            lookout->showStatus(STR_STATUS_STRAFE_L);
            movement->strafe(LEFT_LEFT, strafeDuration);

            // Allow lidar to update
            lookout->showStatus(STR_STATUS_MEASURING);
            sleepOrBackground(LD_FULL_CYCLE_TIME);
          }
          else if (!isStrafeObstructed(RIGHT_RIGHT)) {
            strafeCount++;
            lookout->showStatus( STR_STATUS_STRAFE_R);
            movement->strafe(RIGHT_RIGHT, strafeDuration);

            // Allow lidar to update
            lookout->showStatus(STR_STATUS_MEASURING);
            sleepOrBackground(LD_FULL_CYCLE_TIME);
          }

        }
        else {
          allTheWay = false;
          break;
        }

      }
      else {
        // false reading
        // resume movement
        movement->setDirectionSpeed(true, speed);
      }
    }
    else {
      lookout->showStatus(STR_STATUS_GO_FORWARD);
    }
  }

  return allTheWay;
}

bool ControlMode::driveBackward (int distance, float speed) {
  driveBackward(distance, speed, strafeEnabled());
}

bool ControlMode::driveBackward (int distance, float speed, bool strafeAllowed) {
  lookout->showStatus(STR_STATUS_GO_REVERSE);
  movement->resetPulses();
  movement->resetTripMeter();

  movement->setDirectionSpeed(false, speed);
  bool allTheWay = true;
  while (movement->getDistanceTraveled() < distance) {
    sleepOrBackground(ACTIVE_MOVEMENT_CYCLE_TIME);
    movement->updateSpeeds();

    // Check for obstuctions
    if (isRearObstructed()) {
      movement->stop();

      if (!strafeAllowed) {
        return false;
      }

      lookout->showStatus(STR_STATUS_MEASURING);

      // Allow lidar to run a cycle and check again
      sleepOrBackground(LD_FULL_CYCLE_TIME);
      if (isRearObstructed()) {
        lookout->showStatus(STR_STATUS_OBSTRUCTION_REAR);
        allTheWay = false;
        break;
      }
      else {
        // resume movement
        movement->setDirectionSpeed(false, speed);
      }
    }
  }

  return allTheWay;
}

/*****************
 * Miscellaneous *
 *****************/
void ControlMode::sleepOrBackground(unsigned long sleepTime) {
  lookout->refreshDistances();
  if (runLidar()) {
    // collect measurements for sleep time
    obstructions->collectMeasurements(sleepTime);
  }
  else {
    // do nothing for sleep time
    delay(sleepTime);
  }
  lookout->refreshDistances();
}

bool ControlMode::runLidar() {
  return true;
}

bool ControlMode::strafeEnabled() {
  return true;
}

void ControlMode::logConsole(String msg) {
  if (LOG_ENABLED)
  {
    Serial.println("Log:" + msg);
  }
}

String ControlMode::nameForStrafeDirection (StrafeDirection direction) {
  switch (direction) {
    case LEFT_FORWARD:
      return STR_DIRECTION_LF;
    case LEFT_LEFT:
      return STR_DIRECTION_LL;
    case LEFT_BACKWARD:
      return STR_DIRECTION_LB;
    case RIGHT_FORWARD:
      return STR_DIRECTION_RF;
    case RIGHT_RIGHT:
      return STR_DIRECTION_RR;
    case RIGHT_BACKWARD:
      return STR_DIRECTION_RB;
  }  
  return STR_DIRECTION_UNKNOWN;
}

StrafeDirection ControlMode::directionForStrafeName (String name) {
  if (name.equalsIgnoreCase(STR_DIRECTION_LF)) {
    return LEFT_FORWARD;
  }
  else if (name.equalsIgnoreCase(STR_DIRECTION_LL)) {
    return LEFT_LEFT;
  }
  else if (name.equalsIgnoreCase(STR_DIRECTION_LB)) {
    return LEFT_BACKWARD;
  }
  else if (name.equalsIgnoreCase(STR_DIRECTION_RF)) {
    return RIGHT_FORWARD;
  }
  else if (name.equalsIgnoreCase(STR_DIRECTION_RR)) {
    return RIGHT_RIGHT;
  }
  else if (name.equalsIgnoreCase(STR_DIRECTION_RB)) {
    return RIGHT_BACKWARD;
  }

  return UNKNOWN_DIRECTION;
}