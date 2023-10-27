#include "Commander.h"


void Commander::init() {
  const byte rxPin = 12; // yellow
  const byte txPin = 13; // green

  // Set up a new SoftwareSerial object
  commanderPort = new SoftwareSerial (rxPin, txPin);
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  //commanderPort = &Serial;
  commanderPort->begin(9600);
}

Command Commander::getNextCommand () {
  return receiveCommand ();
}

unsigned long Commander::lastConnected() {
  return lastResponse;
}

bool Commander::isConnected() {
  return (lastResponse > 0);
}

bool Commander::isCompleteCommand(String msg) {
  // should contain COMMAND:str
  // or COMMAND:str|num
  // or SHOWPOSITION:num|num|num
  int delim = msg.indexOf(':');
  int pipeCount = 0;
  String cmd = msg.substring(0, delim);
  if (cmd.length() <= 16) {
    for (int c = delim+1; c < msg.length(); c++) {
      if (msg.charAt(c) == ':') {
        return false;
      }
      else if (msg.charAt(c) == '|') {
        pipeCount++;
      }
    }     
  }

  if (cmd.equalsIgnoreCase("showobstruction") && pipeCount == 1) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("showstatus") && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("showmode") && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("showposition") && pipeCount == 2) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("showcommand") && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("getdistances") && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase("cleardisplay") && pipeCount == 0) {
    return true;
  }
  return false;
}


Command Commander::receiveCommand () {
  // If there is room in the queue, check for another command
  if (commanderPort->available() > 0) {
    lastResponse = millis();

    // read the incoming byte:
    String command = commanderPort->readStringUntil('\n');
    if (isCompleteCommand(command)) {
      String params = "";
      int delim = command.indexOf(':');

      if (delim != -1) {
        params = command.substring(delim+1);
        params.trim();
        command = command.substring(0, delim);
      }

      if (command.equalsIgnoreCase("GETDISTANCES")) {
        Command cmd;
        cmd.setCommandType(GetDistances);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("SHOWSTATUS")) {
        Command cmd;
        cmd.setCommandType(ShowStatus);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("SHOWPOSITION")) {
        Command cmd;
        cmd.setCommandType(ShowPosition);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("SHOWCOMMAND")) {
        Command cmd;
        cmd.setCommandType(ShowCommand);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("SHOWMODE")) {
        Command cmd;
        cmd.setCommandType(ShowMode);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("CLEARDISPLAY")) {
        Command cmd;
        cmd.setCommandType(ClearDisplay);
        cmd.setParams(params);
        return cmd;
      }
      else if (command.equalsIgnoreCase("SHOWOBSTRUCTION")) {
        Command cmd;
        cmd.setCommandType(ShowObstruction);
        cmd.setParams(params);
        return cmd;
      }
      else {
        log("Unknown command " + command);
        //sendResult(Fail);
      }
    }
  }

  // Notify we are ready
  sendPing();

  Command nullCommand;
  nullCommand.setCommandType(Nothing);
  return nullCommand;
}

void Commander::sendResult (CommandResult result) {
  sendPing();
  commanderPort->print("Result:");
  commanderPort->print(result);
  commanderPort->print("\n");
}


void Commander::sendDistances (unsigned long frontLeft, unsigned long frontRight, unsigned long rearLeft, unsigned long rearRight) {
  sendPing();
  //Serial.print("Dist:" + String(frontLeft) + "|" + String(frontRight) + "|" + String(rearLeft) + "|" + String(rearRight) + "\n\n");
  commanderPort->print("Dist:" + String(frontLeft) + "|" + String(frontRight) + "|" + String(rearLeft) + "|" + String(rearRight) + "\n\n");
  commanderPort->flush();
}

void Commander::log(String str) {
  Serial.println(str);
  sendPing();
  commanderPort->print("Log:");
  commanderPort->print(str);
  commanderPort->print("\n");
}

void Commander::sendPing() {
  /*if (millis() - COMMANDER_PING_FREQUENCY > lastPing) {
    commanderPort->print("Ping:" + String(millis())); // regular signal that we are listening
    commanderPort->print("\r\n");
    lastPing = millis();
  }*/
}

CommandType Command::getCommandType () {
  return commandType;
}

void Command::setCommandType (CommandType commandType) {
  this->commandType = commandType;
}

String Command::getParams () {
  return params;
}

void Command::setParams (String params) {
  this->params = params;
}

