#include <Arduino.h>
#include <SoftwareSerial.h>

#define COMMANDER_PING_FREQUENCY 500 // millis between pings

enum CommandType {
  GetDistances = 0,
  ShowStatus = 1,
  ShowPosition =2,
  ShowCommand = 3,
  ShowMode = 4,
  ClearDisplay = 5,
  ShowObstruction = 6,
  Nothing = 7
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
    void init ();
    bool isConnected();
    unsigned long lastConnected();
    Command getNextCommand ();
    void log(String str);
    void sendResult (CommandResult result);
    void sendDistances (unsigned long frontLeft, unsigned long frontRight, unsigned long rearLeft, unsigned long rearRight);
    void sendPing();

  private:
    unsigned long lastResponse = 0;
    Command receiveCommand ();
    unsigned long lastPing = 0;
    SoftwareSerial* commanderPort;
    bool isCompleteCommand(String msg);
};
