#include <Arduino.h>
#include "Obstructions.h"
#include "VehicleMovement.h"
#include "VehicleServos.h"
#include "Commander.h"
#include "Lookout.h"
#include "ControlMode.h"
#include "PilotMode.h"
#include "TestMode.h"
//#include "RandomMode.h"

VehicleMovement movement;
VehicleServos servos;
Obstructions* obstructions;
Commander commander;
Lookout* lookout;
ControlMode* controlMode;

void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial2.begin(COMMANDER_BAUD);
  Serial2.flush();

  logConsole("Starting...");

  int startingTilts[2] = {90,90}; // vertical
  int startingRotations[2] = {0,0}; // forward
  servos.init(startingRotations, startingTilts);
  logConsole("Servos initialized.");

  lookout = new Lookout(&Serial);
  lookout->init();
  logConsole("Lookout initialized");

  obstructions = new Obstructions();
  obstructions->init();
  obstructions->startLidar();
  logConsole("Lidar Initialized");

  // Attach interrupts, unfortunately this can only be done in this sketch, not in the class
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(LEFT_FRONT)), notifyMovementLF, RISING);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(RIGHT_FRONT)), notifyMovementRF, RISING);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(LEFT_REAR)), notifyMovementLR, RISING);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(RIGHT_REAR)), notifyMovementRR, RISING);
  logConsole("Interrupts attached");
}

void loop () {
  controlMode = getControlMode();
  controlMode->loop();
}

ControlMode* getControlMode () {
  logConsole("Getting control mode");
  if (!controlMode) {
    // Chose a control mode
    controlMode = new PilotMode(lookout, obstructions, &movement, &servos, &commander);
    //controlMode = new TestMode(lookout, obstructions, &movement, &servos, &commander);
    controlMode->init();
  }
  return controlMode;
}

// Invoke the appropriate interrupt to increment pulse counts
void notifyMovementLF () {
  movement.interrupt(LEFT_FRONT);
}

void notifyMovementRF () {
  movement.interrupt(RIGHT_FRONT);
}

void notifyMovementLR () {
  movement.interrupt(LEFT_REAR);
}

void notifyMovementRR () {
  movement.interrupt(RIGHT_REAR);
}

void logConsole(String msg) {
  if (LOG_ENABLED) {
    Serial.println("Log:" + msg);
    Serial.flush();
  }
}
