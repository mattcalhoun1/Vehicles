#include "TestMode.h"
void TestMode::init() {
  logConsole("Test mode");

  delay(500);
}

void TestMode::loop() {
  bool testServos = false;
  bool testThermal = true;

  if (testServos) {
    logConsole("Testing servos");

    // Go to each pilot-preferred location, pausing at each
    int wantedRotations[4] = {10, 33, 90, 140};
    for (int r = 0; r < 4; r++) {
        int rightCam = servos->getCameraFor(wantedRotations[r], 90);
        servos->setFrameRelativeRotation(wantedRotations[r], rightCam);
        logConsole(String(rightCam) + ": " + String(wantedRotations[r]));

        int leftCam = servos->getCameraFor(-1 * wantedRotations[r], 90);
        servos->setFrameRelativeRotation(-1 * wantedRotations[r], leftCam);
        logConsole(String(leftCam) + ": " + String(-1 * wantedRotations[r]));
        sleepOrBackground(5000);
    }

    // back to 0
    servos->setFrameRelativeRotation(0, 0);
    servos->setFrameRelativeRotation(0, 1);
    sleepOrBackground(5000);
  }

  if (testThermal) {
    camera->captureImage();
    
    display->showThermal(camera);
  }

  //sleepOrBackground(1000);
}
