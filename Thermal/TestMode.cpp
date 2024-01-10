#include "TestMode.h"
void TestMode::init() {
  logConsole("Test mode");

  delay(500);
}

void TestMode::loop() {
  bool testServos = false;
  bool testThermal = true;
  bool testLora = true;
  
  if (testServos) {
    logConsole("Testing servos");

    // Go to each pilot-preferred location, pausing at each
    int wantedRotations[5] = {30, 60, 90, 120, 150};
    for (int r = 0; r < 5; r++) {
        servos->setFrameRelativeRotation(wantedRotations[r], CAM_MOUNT_LEFT);
        servos->setFrameRelativeRotation(wantedRotations[r], CAM_MOUNT_RIGHT);
        logConsole("Camera Rotations: " + String(wantedRotations[r]));
        sleepOrBackground(5000);
    }

    // back to default
    servos->setFrameRelativeRotation(90, CAM_MOUNT_LEFT);
    servos->setFrameRelativeRotation(90, CAM_MOUNT_RIGHT);
  }

  if (testLora) {
    if (lora->hasMessage()) {
      logConsole("Lora message available. Retrieving.");
      int messageLength = lora->retrieveMessage();
      logConsole("Received " + String(messageLength) + " length message");
    }
    lora->send("Here is a message", LORA_ADDR_REMOTE);
  }
  sleepOrBackground(5000);
}

void TestMode::sleepOrBackground(unsigned long sleepTime) {
  long start = millis();
  while (start + sleepTime > millis()) {
    if (testThermal) {
      camera->captureImage();
      int xOffset = 32;
      int yOffset = 0;
      display->showThermal(camera->getImageData(), camera->getResolutionHeight(), camera->getResolutionWidth(), xOffset, yOffset);

    }
    delay(100);
  }
}