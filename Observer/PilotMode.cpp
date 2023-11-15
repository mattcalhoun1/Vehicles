#include "PilotMode.h"
void PilotMode::init() {
  logConsole(STR_MODE_PILOT);
  display->showMode(STR_MODE_PILOT);
  commander->log("Starting...");  
}


void PilotMode::loop () {
  commander->sendPing();

  Command nextCommand = commander->getNextCommand();
  switch (nextCommand.getCommandType()) {
    case Measure:
      collectMeasurement(nextCommand);
      break;
    case FindMeasurement:
      findMeasurement(nextCommand);
      break;
    case GetConfig: 
      sendConfig(nextCommand);
      break;
    case LidarMap:
      sendLidarMap(nextCommand);
      break;
    case GetCameras:
      sendCameraPositions(nextCommand);
      break;
    case Look:
      executeLook(nextCommand);
      break;
    case ShowPosition:
      showPosition(nextCommand);
      break;
    case ShowObjects:
      showObjects(nextCommand);
      break;
    case ShowStatus:
      showStatus(nextCommand);
      break;
    case ShowCommand:
      showCommand(nextCommand);
      break;
    case ShowMode:
      showMode(nextCommand);
      break;
    case Nothing:
      // send streaming info if configured
      if (streamCameraInfo && (millis() - lastCameraUpdate) > cameraUpdateFrequency) {
        sendCameraPositions(nextCommand);
      }
      else if (streamLidar && (millis() - lastLidarUpdate) > lidarUpdateFrequency) {
        sendLidarMap(nextCommand);
      }
  }

  sleepOrBackground(50);
}

void PilotMode::sendLidarMap (Command cmd) {
  // only update display with command if it isn't an automatic stream
  if (cmd.getCommandType() != Nothing) {
    display->showCommand(STR_COMMAND_MAP);
  }

  lastLidarUpdate = millis();

  // prune the measurements
  unsigned int maxMeasurementAge = RPLIDAR_MEAS_EXPIRE_MILLIS;
  if (!cmd.getParams().equalsIgnoreCase("none")) {
    maxMeasurementAge = cmd.getParams().toInt();
  }
  obstructions->pruneMeasurements(maxMeasurementAge);

  float* lidarMap = obstructions->getDistances();

  if (cmd.getCommandType() != Nothing) {
    commander->sendResult(Success);
  }
  commander->sendMap(obstructions->getLidarHeading(), obstructions->getDistances(), obstructions->getNumMeasurements());
}

void PilotMode::sendConfig (Command cmd) {
  display->showCommand(STR_COMMAND_GET_CONFIG);

  if(cmd.getParams().equalsIgnoreCase(CFG_LIDAR_HEADING)) {
    commander->sendResult(Success);
    commander->sendConfig(CFG_LIDAR_HEADING, obstructions->getLidarHeading());
  }
  else if(cmd.getParams().equalsIgnoreCase(CFG_LIDAR_NUM_MEASUREMENTS)) {
    commander->sendResult(Success);
    commander->sendConfig(CFG_LIDAR_NUM_MEASUREMENTS, obstructions->getNumMeasurements());
  }
  else if(cmd.getParams().equalsIgnoreCase(CFG_LIDAR_GRANULARITY)) {
    commander->sendResult(Success);
    commander->sendConfig(CFG_LIDAR_GRANULARITY, obstructions->getMeasurementsIncrement());
  }
  else {
    commander->sendResult(Fail);
  }
}

void PilotMode::collectMeasurement(Command cmd) {
  display->showCommand(STR_COMMAND_MEASURE);

  // The param should contain angle we want
  float wantedAngle = (cmd.getParams().substring(0, cmd.getParams().indexOf(COMMAND_PARAM_DELIM))).toFloat();
  float wantedTolerance = ((cmd.getParams().substring(cmd.getParams().indexOf(COMMAND_PARAM_DELIM)+1))).toFloat();

  Measurement meas = obstructions->getDistance(wantedAngle, wantedTolerance, 10000);
  commander->sendResult(Success);
  commander->sendMeasurement(meas.angle, meas.distance);
}

void PilotMode::findMeasurement(Command cmd) {
  display->showCommand(STR_COMMAND_FIND_MEASUREMENT);

  String remaining = cmd.getParams();

  float wantedAngle = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float angleTolerance = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float expectedDistance = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float distanceTolerance = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  unsigned long maxAge = remaining.toInt();

  Measurement meas = obstructions->getDistanceInRange(wantedAngle, angleTolerance, expectedDistance, distanceTolerance, maxAge);
  commander->sendResult(Success);
  commander->sendMeasurement(meas.angle, meas.distance);
}

void PilotMode::showPosition(Command cmd) {
  display->showCommand(STR_COMMAND_SHOW_POSITION);

  String remaining = cmd.getParams();

  float x = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float y = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float heading = remaining.toFloat();
  display->showPosition(x, y, heading);
  commander->sendResult(Success);
}

void PilotMode::showObjects(Command cmd) {
  display->showCommand(STR_COMMAND_SHOW_OBJECTS);

  String remaining = cmd.getParams();

  while(remaining.length() > 0) {
    String objectId = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM)));
    remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);
    float estDist = -1.0;
    if (remaining.indexOf(COMMAND_PARAM_DELIM) > 0) {
      estDist = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
    }
    else {
      estDist = remaining.toFloat();
      remaining = "";
    }
    display->showObstruction(objectId, round(estDist));
  }
  commander->sendResult(Success);
}

void PilotMode::showStatus(Command cmd) {
  display->showStatus(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::showCommand(Command cmd) {
  display->showCommand(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::showMode(Command cmd) {
  display->showMode(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::executeLook (Command cmd) {
  display->showCommand(STR_COMMAND_LOOK);
  String remaining = cmd.getParams();
  // Look config is like: ROTATION1|TILT1|ROTATION2|TILT2|...
  // The angles should/could correspond to different cameras.
  while (remaining.length() > 0) {
    if (remaining.indexOf(COMMAND_PARAM_DELIM) != -1) {
      // Execute this part of the 'look'
      int wantedRotation = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toInt();
      remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

      int wantedTilt = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toInt();
      remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

      int camNum = servos->getCameraFor(wantedRotation, wantedTilt);
      servos->setVehicleRelativeRotation(wantedRotation, camNum);
      servos->setCameraTilt(wantedTilt, camNum);
    }
    else {
      remaining = "";
    }
  }

  commander->sendResult(Success);
}

void PilotMode::sendCameraPositions (Command cmd) {
  // only update display with command if it isn't an automatic stream
  if (cmd.getCommandType() != Nothing) {
    display->showCommand(STR_COMMAND_GET_CAMERAS);
  }
  lastCameraUpdate = millis();

  // if number of cameras is increased, this fixed array size must increase
  int rotations[MAX_SUPPORTED_CAMS];
  int tilts[MAX_SUPPORTED_CAMS];
  int minRotations[MAX_SUPPORTED_CAMS];
  int maxRotations[MAX_SUPPORTED_CAMS];
  int minTilts[MAX_SUPPORTED_CAMS];
  int maxTilts[MAX_SUPPORTED_CAMS];

  for (int camCount = 0; camCount < servos->getNumCameras() && camCount < MAX_SUPPORTED_CAMS; camCount++) {
    rotations[camCount] = servos->getVehicleRelativeRotation(camCount);
    tilts[camCount] = servos->getCameraTilt(camCount);
    minRotations[camCount] = servos->getMinVehicleRelativeRotation(camCount);
    maxRotations[camCount] = servos->getMaxVehicleRelativeRotation(camCount);
    minTilts[camCount] = servos->getMinTilt(camCount);
    maxTilts[camCount] = servos->getMaxTilt(camCount);
  }

  if (cmd.getCommandType() != Nothing) {
    commander->sendResult(Success);
  }
  commander->sendCameras (rotations, tilts, minRotations, maxRotations, minTilts, maxTilts, servos->getNumCameras());
}

