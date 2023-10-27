#include "ControlMode.h"

/**
 * This mode is intended for random driving, to help gauge whether the obstacle avoidance is working.
 */
class RandomMode : public ControlMode {
  public:
    RandomMode(Lookout* _lookout, Obstructions* _obstructions, VehicleMovement* _movement, VehicleServos* _servos, Commander* commander) : ControlMode(_lookout, _obstructions, _movement, _servos, commander) {}
    virtual void loop ();
    virtual void init ();

  private:
    void backOut (float speed);
};