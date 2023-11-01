#include "ControlMode.h"
#include "TestModeGlobals.h"

/**
 * This mode is for testing whether the various components of the vehicle are working.
 */
class TestMode : public ControlMode {
  public:
    TestMode(Display* _display, Obstructions* _obstructions, VehicleServos* _servos, Commander* _commander) : ControlMode(_display, _obstructions, _servos, _commander) {}
    virtual void loop ();
    virtual void init ();
    virtual bool runLidar();

  private:
    bool enableLidar = true;
    int minRotations[2] = {-140,0};
    int maxRotations[2] = {0,140};
    bool directionsForward[2] = {false,true};
};