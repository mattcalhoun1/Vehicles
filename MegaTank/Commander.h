#include <Arduino.h>

#define COMMANDER_PING_FREQUENCY 500 // millis between pings

enum CommandType {
  Stop = 0,
  Go = 1,
  Rotate = 2,
  Look = 3,
  Measure = 4, // return measurement near a given location
  LidarMap = 5, // return full lidar map
  GetConfig = 6, // returns a requested config setting
  GetCameras = 7, // returns camera angles visible
  GoForward = 8,
  GoReverse = 9,
  Nothing = 10
};

enum CommandResult {
  Success = 0,
  Fail = 1
};

class Command {
  public:
    virtual CommandType getCommandType ();
    void setCommandType (CommandType commandType);
    String getParams ();
    void setParams(String params);
  protected:
    CommandType commandType;
    String params;
};

class Commander {
  public:
    Command getNextCommand ();
    void log(String str);
    void sendResult (CommandResult result);
    void sendMeasurement (float angle, float distance);
    void sendPing();
    void sendMap(int heading, float* lidarMap, int numMeasurements);
    void sendConfig(String key, String val);
    void sendConfig(String key, int val);
    void sendConfig(String key, float val);
    void sendCameras (int* cameraRotations, int* cameraTilts, int* minRotations, int* maxRotations, int* minTilts, int* maxTilts, int numCameras);
  private:
    Command receiveCommand ();
    unsigned long lastPing = 0;
};
