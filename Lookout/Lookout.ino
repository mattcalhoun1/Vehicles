#include <Arduino.h>
#include "Obstructions.h"
#include "Display.h"
#include "Commander.h"

Obstructions* obstructions = new Obstructions();
Display* display = new Display();
Commander* commander = new Commander();

unsigned long lastLog = 0;

void setup() {
  Serial.begin(9600);
  Serial.flush();
  writeLog("startup");

  display->init();
  display->showStatus("Starting");
  writeLog("display init done");
  
  obstructions->init();
  display->showStatus("Ready");
  writeLog("obstructions init done");

  commander->init();
}

void loop () {
  waitLoop();
  //testLoop();
}

void showPosition(Command cmd) {
  double x = (cmd.getParams().substring(0, cmd.getParams().indexOf("|"))).toDouble();
  String remaining = cmd.getParams().substring(cmd.getParams().indexOf("|")+1);
  double y = (remaining.substring(0, remaining.indexOf("|"))).toDouble();
  remaining = remaining.substring(remaining.indexOf("|")+1);
  double heading = remaining.toDouble();

  display->showPosition(x, y, heading);
}

void showObstruction(Command cmd) {
  String position = (cmd.getParams().substring(0, cmd.getParams().indexOf("|")));
  String remaining = cmd.getParams().substring(cmd.getParams().indexOf("|")+1);
  int dist = remaining.toInt();

  display->showObstruction(position, dist);
}

void sendDistances(Command command) {
  obstructions->scan();
  commander->sendDistances(
    obstructions->getDistanceBumperLeft(), 
    obstructions->getDistanceBumperRight(), 
    obstructions->getDistanceRearBumperLeft(), 
    obstructions->getDistanceRearBumperRight());
}

void showCommand(Command command) {
  display->showCommand(command.getParams());
}

void showMode(Command command) {
  display->showMode(command.getParams());
}

void showStatus(Command command) {
  display->showStatus(command.getParams());
}

void clearDisplay(Command command) {
  display->clearDisplay();
}

void waitLoop () {
  bool connected = commander->isConnected();
  if (!connected) {
    display->showStatus("Uno X Due");
  }

  commander->sendPing();

  Command nextCommand = commander->getNextCommand();
  sendDistances(nextCommand);

  switch (nextCommand.getCommandType()) {
    case GetDistances:
      sendDistances(nextCommand);
      break;
    case ShowCommand: 
      showCommand(nextCommand);
      break;
    case ShowMode:
      showMode(nextCommand);
      break;
    case ShowPosition:
      showPosition(nextCommand);
      break;
    case ShowStatus:
      showStatus(nextCommand);
      break;
    case ClearDisplay:
      clearDisplay(nextCommand);
      break;
    case ShowObstruction:
      showObstruction(nextCommand);
      break;
  }
  if (!connected) {
    display->showCommand("Due: " + String(nextCommand.getCommandType()));
    unsigned long lastConn = commander->lastConnected();
    if (lastConn == 0) {
      display->showMode(String(millis()/1000));
    }
    else {
      display->showMode(String((millis()-lastConn)/1000));
    }
  }

  display->refreshDisplay();

  sleepOrBackground(50);
}


bool testBumpers = true;
void testLoop() {
  //display->showStatus("Running");

  writeLog("current time: " + String(millis()));

  if (testBumpers) {
    display->showStatus("Bumpers");

    // Report distance from all bumpers
    int frontRight = obstructions->getDistanceBumperRight();
    int rearRight = obstructions->getDistanceRearBumperRight();
    int frontLeft = obstructions->getDistanceBumperLeft();
    int rearLeft = obstructions->getDistanceRearBumperLeft();

    writeLog("Distances - FL: " + String(frontLeft) + ", FR: " + String(frontRight) + ", RL: " + String(rearLeft) + ", RR: " + String(rearRight));
    //send("Obstacles:FL=" + String(frontLeft) + "|FR=" + String(frontRight) + "|RL=" + String(rearLeft) + "|RR=" + String(rearRight));
    //writeLog("Distances - FL: " + String(frontLeft) + ", FR: " + String(frontRight));

    if (rearLeft > 0) {
      display->showObstruction("RL", rearLeft);
    }
    if (rearRight > 0) {
      display->showObstruction("RR", rearRight);
    }
    if (frontLeft > 0) {
      display->showObstruction("FL", frontLeft);
    }
    if (frontRight > 0) {
      display->showObstruction("FR", frontRight);
    }
    display->refreshDisplay();
  }    

  sleepOrBackground(1000);
}

void sleepOrBackground(unsigned long sleepTime) {
  // collect measurements for given time
  unsigned long startTime = millis();
  while (sleepTime > (millis() - startTime)) {
    obstructions->scan();
  }

  // do nothing for sleep time
  //delay(sleepTime);
}

void writeLog (String msg) {
  Serial.println(msg);
}


