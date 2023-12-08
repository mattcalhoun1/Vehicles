#include "TestMode.h"
void TestMode::init() {
  logConsole(STR_MODE_TEST);
  lookout->showMode(STR_MODE_TEST);
}

void TestMode::loop() {
  bool movementTestEnabled = true;
  bool stopIfObstructed = true;
  bool servosTestEnabled = true;
  bool lookoutTestEnabled = false;
  bool lidarTestEnabled = false;

  if (lookoutTestEnabled) {
    testLookout();
  }

  if (servosTestEnabled) {
    testServos(stopIfObstructed);
  }

  if (lidarTestEnabled) {
    testLidar();
  }

  if (movementTestEnabled) {
    testMovement(stopIfObstructed);
  }
  sleepOrBackground(1000);
}

void TestMode::testServos (bool stopIfObstructed) {
  lookout->showStatus(STR_STATUS_TEST_SERVOS);

  // Go to each pilot-preferred location, pausing at each
  int wantedRotations[4] = {10, 33, 90, 140};
  for (int r = 0; r < 4; r++) {
    checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
    int rightCam = servos->getCameraFor(wantedRotations[r], 90);
    servos->setVehicleRelativeRotation(wantedRotations[r], rightCam);
    logConsole(String(rightCam) + ": " + String(wantedRotations[r]));

    int leftCam = servos->getCameraFor(-1 * wantedRotations[r], 90);
    servos->setVehicleRelativeRotation(-1 * wantedRotations[r], leftCam);
    logConsole(String(leftCam) + ": " + String(-1 * wantedRotations[r]));
    sleepOrBackground(1000);
  }

  // back to 0
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  servos->setVehicleRelativeRotation(0, 0);
  servos->setVehicleRelativeRotation(0, 1);
  sleepOrBackground(1000);
}

void TestMode::testMovement (bool stopIfObstructed) {
  lookout->showStatus(STR_STATUS_TEST_MOVEMENT);
  movement->resetPulses();

  // Set the direction forward
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  logConsole(STR_STATUS_GO_FORWARD);
  movement->setDirectionSpeed(true, 25.0);
  for (int step = 0; step < 1; step++) {
    sleepOrBackground(1000);
    movement->updateSpeeds();
  }

  // Stop
  movement->stop();
  logConsole(STR_STATUS_STOPPED);
  sleepOrBackground(LD_FULL_CYCLE_TIME);

  // Check the distance
  logConsole(String(movement->getDistanceTraveled()));
  movement->resetTripMeter();

  logConsole(STR_STATUS_GO_REVERSE);
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->setDirectionSpeed(false, 25.0);
  for (int step = 0; step < 1; step++) {
    sleepOrBackground(1000);
    movement->updateSpeeds();
  }

  // Stop
  movement->stop();
  logConsole(STR_STATUS_STOPPED);

  // Check the distance
  logConsole(String(movement->getDistanceTraveled()));
  movement->resetTripMeter();

  // Strafe left
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->strafe(LEFT_LEFT, 2000);

  sleepOrBackground(1000);

  // strafe right
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->strafe(RIGHT_RIGHT, 2000);

  // Rotate -45
  logConsole(STR_STATUS_ROTATE_L);
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->rotate(-45.0);

  sleepOrBackground(1000);

  // rotate +45
  logConsole(STR_STATUS_ROTATE_R);
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->rotate(45.0);
  sleepOrBackground(1000);


  // Rotate -90
  logConsole(STR_STATUS_ROTATE_L);
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->rotate(-90.0);

  sleepOrBackground(1000);

  // rotate +90
  logConsole(STR_STATUS_ROTATE_R);
  checkIfLidarClear(stopIfObstructed); // pause if someone comes near     
  movement->rotate(90.0);
  sleepOrBackground(1000);
}

void TestMode::testLookout () {
  logConsole(STR_STATUS_TEST_LOOKOUT);
  lookout->refreshDistances();

  lookout->showStatus(STR_STATUS_TEST_LOOKOUT);

  // Report distance from all bumpers
  int frontRight = lookout->getDistanceBumperRight();
  int rearRight = lookout->getDistanceRearBumperRight();
  int frontLeft = lookout->getDistanceBumperLeft();
  int rearLeft = lookout->getDistanceRearBumperLeft();

  if (frontLeft > 0 && frontLeft <= US_OBSTRUCTION_THRESHOLD) {
    lookout->showObstruction(STR_FL, frontLeft);
  }
  if (frontRight > 0 && frontRight <= US_OBSTRUCTION_THRESHOLD) {
    lookout->showObstruction(STR_FR, frontRight);
  }
  if (rearLeft > 0 && rearLeft <= US_OBSTRUCTION_THRESHOLD) {
    lookout->showObstruction(STR_RL, rearLeft);
  }
  if (rearRight > 0 && rearRight <= US_OBSTRUCTION_THRESHOLD) {
    lookout->showObstruction(STR_RR, rearRight);
  }  
}

void TestMode::testLidar () {
  lookout->showStatus(STR_STATUS_TEST_LIDAR);
  logConsole(STR_STATUS_TEST_LIDAR);

  sleepOrBackground(LD_FULL_CYCLE_TIME);

  Measurement frontMeas = obstructions->getClosestObstruction(355.0, 5.0, LD_OBSTRUCTION_THRESHOLD, LD_FULL_CYCLE_TIME);
  if (frontMeas.distance > 0.0) {
    lookout->showObstruction(String(frontMeas.angle), (int)frontMeas.distance);
  }

  int numMeasurements = obstructions->getNumMeasurements();
  logConsole("Lidar Map contains " + String(numMeasurements) + " measurements");  
}

void TestMode::checkIfLidarClear (bool sleepUntilClear) {
  if (sleepUntilClear) {
    while (isSomethingNear()) {
      sleepOrBackground(500);
    }
  } 
  else {
    // just display
    isSomethingNear();
  }
}

bool TestMode::isSomethingNear () {
  Measurement m = obstructions->getClosestObstruction(0.0, 360.0, 800.0, 2000); // 31"
  if (m.angle >= 0) {
    lookout->showStatus("Obstacle");
    lookout->showObstruction(getAngleName(m.angle), m.distance);
    return true;
  }
  return false;
}

String TestMode::getAngleName (float angle) {
  if (angle > 330 || angle < 30) {
    return "L";
  }
  else if (angle >= 30 && angle <= 60) {
    return "LF";
  }
  else if (angle > 60 && angle <= 120) {
    return "F";
  }
  else if (angle > 120 && angle <= 150) {
    return "RF";
  }
  else if (angle > 150 && angle <= 210) {
    return "R";
  }
  else if (angle > 210 && angle <= 240) {
    return "RB";
  }
  else if (angle > 240 && angle <= 300) {
    return "B";
  }
  else {
    return "LB";
  }
}

bool TestMode::runLidar () {
  return enableLidar;
}