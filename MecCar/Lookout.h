#include <HardwareSerial.h>
#ifndef LOOKOUT_H
#define LOOKOUT_H

/**
 * Receives obstruction information and sends display
 * information to another device over hardware serial port.
 */
class Lookout {
  public:
    Lookout(HardwareSerial* _lookoutPort) : lookoutPort(_lookoutPort) {}
    void init ();

    // display
    void showObstruction (String location, int distance);
    void showStatus (String status);
    void showPosition (double x, double y, double heading);
    void showCommand (String command);
    void showMode (String mode);
    void clearDisplay ();

    // obstructions
    bool bumperSensorsEnabled();
    void refreshDistances();
    unsigned long getDistanceBumperLeft();
    unsigned long getDistanceBumperRight();
    unsigned long getDistanceRearBumperLeft();
    unsigned long getDistanceRearBumperRight();

  private:
    HardwareSerial* lookoutPort;
    unsigned long lastLeft = 0;
    unsigned long lastRight = 0;
    unsigned long lastRearLeft = 0;
    unsigned long lastRearRight = 0;
    unsigned long distanceAge = 0;
    String readLatestMeasurements();
    bool isCompleteMeasurement(String msg);
};

#endif