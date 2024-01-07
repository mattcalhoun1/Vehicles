#include "ControlMode.h"

ControlMode::ControlMode(Display* _display, Camera* _camera, CameraServos* _servos, LoRaTrans* _lora) {
  display = _display;
  servos = _servos;
  camera = _camera;
  lora = _lora;
}

/*****************
 * Miscellaneous *
 *****************/
void ControlMode::sleepOrBackground(unsigned long sleepTime) {
  delay(sleepTime);
}

void ControlMode::logConsole(String msg) {
  if (LOG_ENABLED)
  {
    Serial.println("Log:" + msg);
  }
}
