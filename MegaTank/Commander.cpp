#include "Commander.h"


Command Commander::getNextCommand () {
  return receiveCommand ();
}

Command Commander::receiveCommand () {
  // If there is room in the queue, check for another command
  if (Serial2.available() > 0) {
    // read the incoming byte:
    String command = Serial2.readString();
    String params = "";
    int delim = command.indexOf(':');

    if (delim != -1) {
      params = command.substring(delim+1);
      params.trim();
      command = command.substring(0, delim);
    }

    if (command.equalsIgnoreCase("GO")) {
      Command go;
      go.setCommandType(Go);
      go.setParams(params);
      return go;
    }
    else if (command.equalsIgnoreCase("FORWARD")) {
      Command forward;
      forward.setCommandType(GoForward);
      forward.setParams(params);
      return forward;
    }
    else if (command.equalsIgnoreCase("REVERSE")) {
      Command reverse;
      reverse.setCommandType(GoReverse);
      reverse.setParams(params);
      return reverse;
    }
    else if (command.equalsIgnoreCase("STOP")) {
      Command stop;
      stop.setCommandType(Stop);
      stop.setParams(params);
      return stop;
    }
    else if (command.equalsIgnoreCase("ROTATE")) {
      Command rotate;
      rotate.setCommandType(Rotate);
      rotate.setParams(params);
      return rotate;
    }
    else if (command.equalsIgnoreCase("LOOK")) {
      Command look;
      look.setCommandType(Look);
      look.setParams(params);
      return look;
    }
    else if (command.equalsIgnoreCase("MEASURE")) {
      Command measure;
      measure.setCommandType(Measure);
      measure.setParams(params);
      return measure;
    }
    else if (command.equalsIgnoreCase("MAP")) {
      Command lidarMap;
      lidarMap.setCommandType(LidarMap);
      lidarMap.setParams(params);
      return lidarMap;
    }
    else if (command.equalsIgnoreCase("GETCONFIG")) {
      Command getConfig;
      getConfig.setCommandType(GetConfig);
      getConfig.setParams(params);
      return getConfig;
    }
    else if (command.equalsIgnoreCase("GETCAMERAS")) {
      Command getCams;
      getCams.setCommandType(GetCameras);
      getCams.setParams(params);
      return getCams;
    }
    else {
      log("Unknown command " + command);
      sendResult(Fail);
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
  Serial2.print("Result:");
  Serial2.print(result);
  Serial2.print("\n");
}

void Commander::sendMeasurement(float angle, float measurement) {
  sendPing();
  Serial2.print("Measurement:");
  Serial2.print(String(angle));
  Serial2.print("|");
  Serial2.print(String(measurement));
  Serial2.print("\n");
}

void Commander::sendMap(int heading, float* lidarMap, int numMeasurements) {
  sendPing();
  Serial2.print("Map:");
  for (int m = 0; m < numMeasurements; m++) {
    Serial2.print(lidarMap[m]);
    if (m+1 < numMeasurements) {
      Serial2.print("|");
    }
  }
  Serial2.print("\n");
}

void Commander::sendConfig(String key, String val) {
  sendPing();
  Serial2.print("Config:");
  Serial2.print(key);
  Serial2.print("|");
  Serial2.print(val);
  Serial2.print("\n");

  Serial.print("Config:");
  Serial.print(key);
  Serial.print("|");
  Serial.print(val);
  Serial.print("\n");

}

void Commander::sendConfig(String key, int val) {
  sendPing();
  Serial2.print("Config:");
  Serial2.print(key);
  Serial2.print("|");
  Serial2.print(val);
  Serial2.print("\n");
}

void Commander::sendConfig(String key, float val) {
  sendPing();
  Serial2.print("Config:");
  Serial2.print(key);
  Serial2.print("|");
  Serial2.print(val);
  Serial2.print("\n");
}

void Commander::sendCameras (int* cameraRotations, int* cameraTilts, int* minRotations, int* maxRotations, int* minTilts, int* maxTilts, int numCameras) {
  sendPing();
  // Cameras:[rotation1]|[tilt1]|[minrotation1]|[maxrotation1]|[mintilt1]|[maxtilt1],[rotation2]|...
  Serial2.print("Cameras:");
  for (int camCount = 0; camCount < numCameras; camCount++) {
    Serial2.print(cameraRotations[camCount]);
    Serial2.print("|");
    Serial2.print(cameraTilts[camCount]);
    Serial2.print("|");
    Serial2.print(minRotations[camCount]);
    Serial2.print("|");
    Serial2.print(maxRotations[camCount]);
    Serial2.print("|");
    Serial2.print(minTilts[camCount]);
    Serial2.print("|");
    Serial2.print(maxTilts[camCount]);

    if (camCount + 1 < camCount) {
      Serial2.print(",");
    }
  }
  Serial2.print("\n");
}

void Commander::log(String str) {
  sendPing();
  Serial2.print("Log:");
  Serial2.print(str);
  Serial2.print("\n");
}

void Commander::sendPing() {
  if (millis() - COMMANDER_PING_FREQUENCY > lastPing) {
    Serial2.print("!READY!"); // regular signal to linux/python that we are listening
    Serial2.print("\n");
    lastPing = millis();
  }
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

