#include <Arduino.h>
#include "Display.h"
#include "CameraServos.h"
#include "Camera.h"
#include "LoRaTrans.h"
#include "Globals.h"

#ifndef CONTROL_MODE_H
#define CONTROL_MODE_H

/**
 * Base class for the different control modes available for this vehicle.
 */
class ControlMode {
  public:
    ControlMode(Display* _display, Camera* _camera, CameraServos* _servos, LoRaTrans* _lora);
    virtual void loop() = 0;
    virtual void init() = 0;

  protected:
    // miscellaneous
    void logConsole(String msg);
    void sleepOrBackground(unsigned long sleepTime);

    Display* display;
    CameraServos* servos;
    Camera* camera;
    LoRaTrans* lora;
};
#endif