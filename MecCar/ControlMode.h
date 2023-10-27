#include <Arduino.h>
#include "Obstructions.h"
#include "VehicleMovement.h"
#include "VehicleServos.h"
#include "Commander.h"
#include "Lookout.h"
#include "Globals.h"

#ifndef CONTROL_MODE_H
#define CONTROL_MODE_H

/**
 * Base class for the different control modes available for this vehicle.
 */
class ControlMode {
  public:
    ControlMode(Lookout* _lookout, Obstructions* _obstructions, VehicleMovement* _movement, VehicleServos* _servos, Commander* commander);
    virtual void loop() = 0;
    virtual void init() = 0;

  protected:
    // sensing
    bool isObstructed();
    bool isRearObstructed();
    bool isStrafeObstructed(StrafeDirection direction);

    // movement
    bool driveBackward (int distance, float speed);
    bool driveBackward (int distance, float speed, bool strafeAllowed);
    bool driveForward (int distance, float speed);
    bool driveForward (int distance, float speed, bool strafeAllowed);
    bool strafe (StrafeDirection direction, int duration);

    // miscellaneous
    void logConsole(String msg);
    void sleepOrBackground(unsigned long sleepTime);
    virtual bool runLidar();
    virtual bool strafeEnabled();
    String nameForStrafeDirection(StrafeDirection direction);
    StrafeDirection directionForStrafeName (String name);

    Lookout* lookout;
    Obstructions* obstructions;
    VehicleMovement* movement;
    VehicleServos* servos;
    Commander* commander;
};
#endif