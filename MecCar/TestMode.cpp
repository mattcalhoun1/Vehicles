#include "TestMode.h"
void TestMode::init() {
  logConsole(STR_MODE_TEST);
  lookout->showMode(STR_MODE_TEST);
}

void TestMode::loop() {
  bool testMovement = false;
  bool testServos = true;
  bool testLookout = false;
  bool testLidar = false;

  if (testLookout) {
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

  if (testServos) {
    lookout->showStatus(STR_STATUS_TEST_SERVOS);
    logConsole(STR_STATUS_TEST_SERVOS);
    for (int cam = 0; cam < 2; cam++) {
      //commander.log("getting vehicle rotation for cam " + String(cam));
      int nextAngle = servos->getVehicleRelativeRotation(cam);
      logConsole(String(cam) + ": " + String(nextAngle));
      if (directionsForward[cam]) {
        nextAngle += 25;
        if (nextAngle > maxRotations[cam]) {
          nextAngle = maxRotations[cam] - 25;
          directionsForward[cam] = !directionsForward[cam];
        }
      }
      else {
        nextAngle -= 25;
        if (nextAngle < minRotations[cam]) {
          nextAngle = minRotations[cam] + 25;
          directionsForward[cam] = !directionsForward[cam];
        }
      }
      servos->setVehicleRelativeRotation(nextAngle, cam);
      sleepOrBackground(1000);
    }
  }

  if (testLidar) {
    lookout->showStatus(STR_STATUS_TEST_LIDAR);
    logConsole(STR_STATUS_TEST_LIDAR);

    sleepOrBackground(LD_FULL_CYCLE_TIME);

    Measurement frontMeas = obstructions->getClosestObstruction(355.0, 5.0, LD_OBSTRUCTION_THRESHOLD);
    if (frontMeas.distance > 0.0) {
      lookout->showObstruction(String(frontMeas.angle), (int)frontMeas.distance);
    }

    /*Measurement rearMeas = obstructions->getClosestObstruction(175.0, 185.0, lidarObstructionThreshold);
    if (rearMeas.distance > 0.0) {
      lookout->showObstruction(String(rearMeas.angle), (int)rearMeas.distance);
    }*/
  }

  if (testMovement) {
    lookout->showStatus(STR_STATUS_TEST_MOVEMENT);
    movement->resetPulses();

    // Set the direction forward
    logConsole(STR_STATUS_GO_FORWARD);
    movement->setDirectionSpeed(true, 15.0);
    for (int step = 0; step < 3; step++) {
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
    movement->setDirectionSpeed(false, 15.0);
    for (int step = 0; step < 3; step++) {
      sleepOrBackground(1000);
      movement->updateSpeeds();
    }

    // Stop
    movement->stop();
    logConsole(STR_STATUS_STOPPED);

    // Check the distance
    logConsole(String(movement->getDistanceTraveled()));
    movement->resetTripMeter();

    // Rotate -90
    logConsole(STR_STATUS_ROTATE_L);
    movement->rotate(-90.0);

    sleepOrBackground(1000);

    // rotate +90
    logConsole(STR_STATUS_ROTATE_R);
    movement->rotate(90.0);
  }

  sleepOrBackground(1000);
}

bool TestMode::runLidar () {
  return enableLidar;
}