#include "WatchMode.h"

void WatchMode::init() {
  logConsole("Watch mode");

  // Move servos to default positions
  servos->setFrameRelativeRotation(defaultLeftRotation, CAM_MOUNT_LEFT);
  servos->setFrameRelativeRotation(defaultRightRotation, CAM_MOUNT_RIGHT);

  captureThermal[CAM_MOUNT_LEFT] = false;
  captureThermal[CAM_MOUNT_RIGHT] = true;

  loraCommander = new LoRaCommander(lora, LORA_ADDR_REMOTE);
  loraCommander->init();

  display->clear();
  //delay(500);
}

CommandResult WatchMode::executeCommand (Command command) {
  logConsole("Executing " + String(command.getId()));


  if (command.getCommandType() == GetThermal) {
    return sendThermal(command.getParams().toInt());
  }

  return Success;
}


CommandResult WatchMode::sendThermal (int cameraId) {
  if(encoder.encode(camera->getImageData())) {
    logConsole("Sending encoded image size " + String(encoder.getEncodedBufferLength()));
    // Send a ping
    lora->send(encoder.getEncodeDecodeBuffer(), encoder.getEncodedBufferLength(), LORA_ADDR_REMOTE);
    logConsole("image sent");
    return Success;
  }
  else {
    logConsole("Image encoding failed");
    return Fail;
  }
}

void WatchMode::loop() {
  sleepOrBackground(100);
  
  if (loraEnabled) {
    // Check for any commands or pings received via lora
    Command loraCommand = loraCommander->getNextCommand();
    if (loraCommand.getCommandType() != Nothing) {
      // Acknowledge receipt of the command
      loraCommander->sendReceipt(loraCommand, Ok);

      // Execute the command
      CommandResult result = executeCommand(loraCommand);

      // send the result
      loraCommander->sendResult(loraCommand, result);
    }

    // ping the remote occasionally, if configured
    if (remotePingEnabled) {
      loraCommander->sendPing();
    }
  }
  sleepOrBackground(100);
}

void WatchMode::sleepOrBackground(unsigned long sleepTime) {
  long start = millis();
  while (start + sleepTime > millis()) {
    if (captureThermal[CAM_MOUNT_RIGHT]) {
      camera->captureImage();
      int xOffset = 12;
      int yOffset = 0;
      //display->clear();
      //display->showThermal(camera->getImageData(), camera->getResolutionHeight(), camera->getResolutionWidth(), xOffset, yOffset);

      //#define THERMAL_INTERPOLATED_WIDTH 64
      //#define THERMAL_INTERPOLATED_HEIGHT 48

      // interpolate
      for (int r = 0; r < THERMAL_INTERPOLATED_HEIGHT; r++) {
        float* interpolatedRow = encoder.getInterpolatedRow(camera->getImageData(), r);
        display->showInterpolatedThermalRow(interpolatedRow, xOffset, r + yOffset);
      }
     
    }
    delay(100);
  }
}