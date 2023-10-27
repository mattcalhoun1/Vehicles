#include "RandomMode.h"
void RandomMode::init() {
  logConsole(STR_MODE_RANDOM);
  lookout->showMode(STR_MODE_RANDOM);
}

void RandomMode::loop() {
  float speed = 5.0;

  // cameras forward
  servos->setCameraTilt(90.0, 0);
  servos->setVehicleRelativeRotation(0.0, 0);
  servos->setVehicleRelativeRotation(0.0, 1);

  // make a random turn, as long as the path is not obstructed
  if (driveForward(36, speed, true)) {
    // Stop
    movement->stop();
    logConsole(STR_STATUS_STOPPING);
  }
  else {
    lookout->showStatus(STR_STATUS_OBSTRUCTION_FRONT);
    backOut(speed);
    logConsole(STR_STATUS_BACKED_OUT);
  }

  // allow time for lidar to catch up
  sleepOrBackground(LD_FULL_CYCLE_TIME);
}

void RandomMode::backOut (float speed) {
  float totalRotated = 0.0;

  // We should already be stopped
  // check for rear obstructions
  while (isRearObstructed()) {
    // Rotate a little
    logConsole(STR_STATUS_OBSTRUCTION_REAR);
    lookout->showStatus(STR_STATUS_OBSTRUCTION_REAR);
    movement->rotate(75.0);
    totalRotated += 75.0;

    sleepOrBackground(LD_FULL_CYCLE_TIME); // give time to update lidar
  }

  if (driveBackward(12, speed)) {
    movement->stop();
  }
}