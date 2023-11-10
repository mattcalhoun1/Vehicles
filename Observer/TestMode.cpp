#include "TestMode.h"
void TestMode::init() {
  logConsole(STR_MODE_TEST);
  display->showMode(STR_MODE_TEST);

  delay(5000);
}

void TestMode::loop() {
  bool testServos = true;
  bool testLidar = false;

  if (testServos) {
    display->showStatus(STR_STATUS_TEST_SERVOS);

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
    display->showStatus(STR_STATUS_TEST_LIDAR);
    logConsole(STR_STATUS_TEST_LIDAR);

    sleepOrBackground(LD_FULL_CYCLE_TIME);

    Measurement frontMeas = obstructions->getClosestObstruction(355.0, 5.0, LD_OBSTRUCTION_THRESHOLD);
    if (frontMeas.distance > 0.0) {
      display->showObstruction(String(frontMeas.angle), (int)frontMeas.distance);
    }

    Measurement rearMeas = obstructions->getClosestObstruction(175.0, 185.0, LD_OBSTRUCTION_THRESHOLD);
    if (rearMeas.distance > 0.0) {
      display->showObstruction(String(rearMeas.angle), (int)rearMeas.distance);
    }
  }

  sleepOrBackground(1000);
}

bool TestMode::runLidar () {
  return enableLidar;
}