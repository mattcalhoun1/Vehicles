#include "WatchMode.h"

void WatchMode::init() {
  logConsole("Watch mode");

  // Move servos to default positions
  servos->setFrameRelativeRotation(defaultLeftRotation, CAM_MOUNT_LEFT);
  servos->setFrameRelativeRotation(defaultRightRotation, CAM_MOUNT_RIGHT);

  captureThermal[CAM_MOUNT_LEFT] = false;
  captureThermal[CAM_MOUNT_RIGHT] = true;

  //delay(500);
}

void WatchMode::loop() {
  sleepOrBackground(1000);
  
  if (loraEnabled) {
    if (lora->hasMessage()) {
      logConsole("Lora message available. Retrieving.");
      int messageLength = lora->retrieveMessage();
      logConsole("Received LORA message: " + String((char*)lora->getMessageBuffer()));
    }
    //lora->send("Here is a message", LORA_ADDR_REMOTE);

    // ping the remote occasionally, if configured
    if (remotePingEnabled) {
      if (remoteLastPing + remotePingFrequency < millis()) {
        remoteLastPing = millis();
        if(encoder.encode(camera->getImageData())) {
          logConsole("Sending encoded image size " + String(encoder.getEncodedBufferLength()));
          // Send a ping
          lora->send(encoder.getEncodeDecodeBuffer(), encoder.getEncodedBufferLength(), LORA_ADDR_REMOTE);
          logConsole("image sent");
        }
        else {
          logConsole("Image encoding failed");
        }
      }
    }
  }
  sleepOrBackground(4000);
}

void WatchMode::sleepOrBackground(unsigned long sleepTime) {
  long start = millis();
  while (start + sleepTime > millis()) {
    if (captureThermal[CAM_MOUNT_RIGHT]) {
      camera->captureImage();
      display->showThermal(camera);
    }
    delay(100);
  }
}