#include "TestMode.h"
void TestMode::init() {
  logConsole(STR_MODE_TEST);
  display->showMode(STR_MODE_TEST);

  delay(5000);
}

void TestMode::loop() {
  bool testServos = true;
  bool testLidar = true;

  if (testServos) {
    display->showStatus(STR_STATUS_TEST_SERVOS);
    logConsole(STR_STATUS_TEST_SERVOS);
    for (int nextAngle = 0; nextAngle < 180; nextAngle+= 30) {
      for (int cam = 0; cam < 2; cam++) {
        CameraMountPosition pos = servos->getMountPosition(cam);
        int selectedAngle = nextAngle;
        if (pos == CAM_MOUNT_LEFT) {
          selectedAngle *= -1;
        }

        servos->setVehicleRelativeRotation(selectedAngle, cam);
        sleepOrBackground(1000);
      }
    }
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