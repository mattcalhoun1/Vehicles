#include "ControlMode.h"
#include <math.h>

#define STR_UNKNOWN_DIRECTION "Unknown direction"

#define STR_COMMAND_GO_REVERSE "GoReverse"
#define STR_COMMAND_GO_FORWARD "GoForward"
#define STR_COMMAND_STOP "Stop"
#define STR_COMMAND_ROTATE "Rotate"
#define STR_COMMAND_STRAFE "Strafe"
#define STR_COMMAND_GET_CAMERAS "GetCameras"
#define STR_COMMAND_LOOK "Look"
#define STR_COMMAND_MAP "Map"
#define STR_COMMAND_MEASURE "Measure"
#define STR_COMMAND_FIND_MEASUREMENT "FindMeasurement"
#define STR_COMMAND_GET_CONFIG "GetConfig"
#define STR_COMMAND_SHOW_POSITION "ShowPosition"
#define STR_COMMAND_SHOW_OBJECTS "ShowObjects"

#define CFG_LIDAR_HEADING "LidarHeading"
#define CFG_LIDAR_NUM_MEASUREMENTS "LidarNumMeasurements"
#define CFG_LIDAR_GRANULARITY "LidarGranularity"

#define COMMAND_PARAM_DELIM "|"

#define MAX_SUPPORTED_CAMS 2 // controls how many cameras can be controlled via commander

/**
 * This mode is for being 'piloted' by another device over USB (raspberry pi, nvidia jetson nano, etc).
 * It essentially waits for commands from the pilot.
 */
class PilotMode : public ControlMode {
  public:
    PilotMode(Lookout* _lookout, Obstructions* _obstructions, VehicleMovement* _movement, VehicleServos* _servos, Commander* commander) : ControlMode(_lookout, _obstructions, _movement, _servos, commander) {}
    virtual void loop ();
    virtual void init ();

  private:
    void sendLidarMap (Command cmd);
    void sendConfig (Command cmd);
    void collectMeasurement(Command cmd);
    void findMeasurement(Command cmd);
    void executeLook (Command cmd);
    void sendCameraPositions (Command cmd);
    void executeRotate (Command cmd);
    void executeGoForward (Command cmd);
    void executeGoReverse (Command cmd);
    void executeStrafe (Command cmd);
    void executeStop (Command cmd);
    void showPosition (Command cmd);
    void showObjects (Command cmd);
    void showStatus(Command cmd);
    void showCommand(Command cmd);
    void showMode(Command cmd);

    bool streamCameraInfo = false;
    bool streamLidar = false;

    unsigned long lastCameraUpdate = 0;
    unsigned long lastLidarUpdate = 0;
    unsigned long lidarUpdateFrequency = 5000;
    unsigned long cameraUpdateFrequency = 4000;
};