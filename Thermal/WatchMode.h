#include "ControlMode.h"
#include "ThermalEncoder.h"
#include "Commander.h"
#include "LoRaCommander.h"

/**
 * This mode is for watching an area for activity
 */
class WatchMode : public ControlMode {
  public:
    WatchMode(Display* _display, Camera* _camera, CameraServos* _servos, LoRaTrans* _lora) : ControlMode(_display, _camera, _servos, _lora) {}
    virtual void loop ();
    virtual void init ();
    virtual void sleepOrBackground(unsigned long sleepTime);

  private:
    bool loraEnabled = true;
    bool remotePingEnabled = true;
    long remotePingFrequency = 30000; // every 30 sec
    unsigned long remoteLastPing = 0;
    bool captureThermal[2];
    bool cycleServos = false;
    int defaultLeftRotation = 60;
    int defaultRightRotation = 120;
    int minRotations[2] = {-140,0};
    int maxRotations[2] = {0,140};

    CommandResult executeCommand (Command command);

    ThermalEncoder encoder = ThermalEncoder(THERMAL_HEIGHT, THERMAL_WIDTH, false);
    Commander* loraCommander;
};