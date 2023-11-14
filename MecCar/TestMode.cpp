#include "TestMode.h"
void TestMode::init() {
  logConsole(STR_MODE_TEST);
  lookout->showMode(STR_MODE_TEST);
}

void TestMode::loop() {
  bool testMovement = false;
  bool testServos = false;
  bool testLookout = false;
  bool testLidar = true;

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

    // Go to each pilot-preferred location, pausing at each
    int wantedRotations[4] = {10, 33, 90, 140};
    for (int r = 0; r < 4; r++) {
        int rightCam = servos->getCameraFor(wantedRotations[r], 90);
        servos->setVehicleRelativeRotation(wantedRotations[r], rightCam);
        logConsole(String(rightCam) + ": " + String(wantedRotations[r]));

        int leftCam = servos->getCameraFor(-1 * wantedRotations[r], 90);
        servos->setVehicleRelativeRotation(-1 * wantedRotations[r], leftCam);
        logConsole(String(leftCam) + ": " + String(-1 * wantedRotations[r]));
        sleepOrBackground(5000);
    }

    // back to 0
    servos->setVehicleRelativeRotation(0, 0);
    servos->setVehicleRelativeRotation(0, 1);
    sleepOrBackground(5000);
  }

  if (testLidar) {
    lookout->showStatus(STR_STATUS_TEST_LIDAR);
    logConsole(STR_STATUS_TEST_LIDAR);

    sleepOrBackground(LD_FULL_CYCLE_TIME);

    Measurement frontMeas = obstructions->getClosestObstruction(355.0, 5.0, LD_OBSTRUCTION_THRESHOLD);
    if (frontMeas.distance > 0.0) {
      lookout->showObstruction(String(frontMeas.angle), (int)frontMeas.distance);
    }

    int numMeasurements = obstructions->getNumMeasurements();
    logConsole("Lidar Map contains " + String(numMeasurements) + " measurements");
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