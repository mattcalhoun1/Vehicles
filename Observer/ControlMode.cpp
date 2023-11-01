#include "ControlMode.h"

ControlMode::ControlMode(Display* _display, Obstructions* _obstructions, VehicleServos* _servos, Commander* _commander) {
  display = _display;
  obstructions = _obstructions;
  servos = _servos;
  commander = _commander;
}

/*****************
 * Miscellaneous *
 *****************/
void ControlMode::sleepOrBackground(unsigned long sleepTime) {
  if (runLidar()) {
    // collect measurements for sleep time
    obstructions->collectMeasurements(sleepTime);
  }
  else {
    // do nothing for sleep time
    delay(sleepTime);
  }
}

bool ControlMode::runLidar() {
  return true;
}

void ControlMode::logConsole(String msg) {
  if (LOG_ENABLED)
  {
    Serial.println("Log:" + msg);
  }
}
