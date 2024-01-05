#include "ControlMode.h"

/**
 * This mode is for testing whether the various components of the vehicle are working.
 */
class TestMode : public ControlMode {
  public:
    TestMode(Display* _display, Camera* _camera, CameraServos* _servos) : ControlMode(_display, _camera, _servos) {}
    virtual void loop ();
    virtual void init ();
    virtual void sleepOrBackground(unsigned long sleepTime);

  private:
    int minRotations[2] = {-140,0};
    int maxRotations[2] = {0,140};
    bool testThermal = true;
    bool testServos = true;
};