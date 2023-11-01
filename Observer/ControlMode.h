#include <Arduino.h>
#include "Display.h"
#include "Obstructions.h"
#include "VehicleServos.h"
#include "Commander.h"
#include "Globals.h"

#ifndef CONTROL_MODE_H
#define CONTROL_MODE_H

/**
 * Base class for the different control modes available for this vehicle.
 */
class ControlMode {
  public:
    ControlMode(Display* _display, Obstructions* _obstructions, VehicleServos* _servos, Commander* commander);
    virtual void loop() = 0;
    virtual void init() = 0;

  protected:
    // miscellaneous
    void logConsole(String msg);
    void sleepOrBackground(unsigned long sleepTime);
    virtual bool runLidar();

    Display* display;
    Obstructions* obstructions;
    VehicleServos* servos;
    Commander* commander;
};
#endif