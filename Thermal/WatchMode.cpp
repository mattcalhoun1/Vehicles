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

  //delay(500);
}

CommandResult WatchMode::executeCommand (Command command) {
  logConsole("Executing " + String(command.getId()));
  return Success;
}


void WatchMode::loop() {
  sleepOrBackground(1000);
  
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

    /*
    if (lora->hasMessage()) {
      logConsole("Lora message available. Retrieving.");
      int messageLength = lora->retrieveMessage();
      logConsole("Received LORA message: " + String((char*)lora->getMessageBuffer()));


    }
    //lora->send("Here is a message", LORA_ADDR_REMOTE);
    */

    // ping the remote occasionally, if configured
    if (remotePingEnabled) {
      loraCommander->sendPing();
      /*if (remoteLastPing + remotePingFrequency < millis()) {
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
      }*/
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