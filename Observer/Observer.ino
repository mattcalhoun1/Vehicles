#include <Arduino.h>
#include "Obstructions.h"
#include "VehicleServos.h"
#include "Display.h"
#include "Commander.h"
#include "ControlMode.h"
#include "PilotMode.h"
#include "TestMode.h"

VehicleServos servos;
Obstructions* obstructions;
Commander commander;
ControlMode* controlMode;
Display* display = new Display();

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

  display->init();
  display->showStatus("Starting");
  logConsole("Display Initialized");

  obstructions = new Obstructions();
  obstructions->init();
  obstructions->startLidar();
  logConsole("Lidar Initialized");
}

void loop () {
  controlMode = getControlMode();
  controlMode->loop();
}

ControlMode* getControlMode () {
  if (!controlMode) {
    logConsole("Getting control mode");
    // Chose a control mode
    controlMode = new PilotMode(display, obstructions, &servos, &commander);
    //controlMode = new TestMode(display, obstructions, &servos, &commander);
    controlMode->init();
  }
  return controlMode;
}

void logConsole(String msg) {
  if (LOG_ENABLED) {
    Serial.println("Log:" + msg);
    Serial.flush();
  }
}
