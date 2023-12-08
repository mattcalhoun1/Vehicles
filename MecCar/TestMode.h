#include "ControlMode.h"
#include "TestModeGlobals.h"

/**
 * This mode is for testing whether the various components of the vehicle are working.
 */
class TestMode : public ControlMode {
  public:
    TestMode(Lookout* _lookout, Obstructions* _obstructions, VehicleMovement* _movement, VehicleServos* _servos, Commander* _commander) : ControlMode(_lookout, _obstructions, _movement, _servos, _commander) {}
    virtual void loop ();
    virtual void init ();
    virtual bool runLidar();

  private:
    bool enableLidar = true;
    int minRotations[2] = {-140,0};
    int maxRotations[2] = {0,140};
    bool directionsForward[2] = {false,true};
    String getAngleName (float angle);
    bool isSomethingNear ();
    void checkIfLidarClear (bool sleepUntilClear);

    void testServos (bool stopIfObstructed);
    void testMovement (bool stopIfObstructed);
    void testLookout ();
    void testLidar ();
};