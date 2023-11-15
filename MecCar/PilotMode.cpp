#include "PilotMode.h"
void PilotMode::init() {
  logConsole(STR_MODE_PILOT);
  lookout->showMode(STR_MODE_PILOT);
  commander->log("Starting...");  
}


void PilotMode::loop () {
  // make sure vehicle is stopped
  movement->stop();
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
    case Stop:
      executeStop(nextCommand);
      break;
    case GoReverse:
      executeGoReverse(nextCommand);
      break;
    case GoForward:
      executeGoForward(nextCommand);
      break;
    case Strafe:
      executeStrafe(nextCommand);
      break;
    case Rotate:
      executeRotate(nextCommand);
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
    lookout->showCommand(STR_COMMAND_MAP);
  }

  lastLidarUpdate = millis();

  // prune the measurements
  int maxMeasurementAge = RPLIDAR_MEAS_EXPIRE_MILLIS;
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
  lookout->showCommand(STR_COMMAND_GET_CONFIG);

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
  lookout->showCommand(STR_COMMAND_MEASURE);

  // The param should contain angle we want
  float wantedAngle = (cmd.getParams().substring(0, cmd.getParams().indexOf(COMMAND_PARAM_DELIM))).toFloat();
  float wantedTolerance = ((cmd.getParams().substring(cmd.getParams().indexOf(COMMAND_PARAM_DELIM)+1))).toFloat();

  Measurement meas = obstructions->getDistance(wantedAngle, wantedTolerance, 10000);
  commander->sendResult(Success);
  commander->sendMeasurement(meas.angle, meas.distance);
}

void PilotMode::findMeasurement(Command cmd) {
  lookout->showCommand(STR_COMMAND_FIND_MEASUREMENT);

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
  lookout->showCommand(STR_COMMAND_SHOW_POSITION);

  String remaining = cmd.getParams();

  float x = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float y = (remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM))).toFloat();
  remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);

  float heading = remaining.toFloat();
  lookout->showPosition(x, y, heading);
  commander->sendResult(Success);
}

void PilotMode::showObjects(Command cmd) {
  lookout->showCommand(STR_COMMAND_SHOW_OBJECTS);

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
    lookout->showObstruction(objectId, round(estDist));
  }
  commander->sendResult(Success);
}

void PilotMode::showStatus(Command cmd) {
  lookout->showStatus(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::showCommand(Command cmd) {
  lookout->showCommand(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::showMode(Command cmd) {
  lookout->showMode(cmd.getParams());
  commander->sendResult(Success);
}

void PilotMode::executeLook (Command cmd) {
  lookout->showCommand(STR_COMMAND_LOOK);
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
    lookout->showCommand(STR_COMMAND_GET_CAMERAS);
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

void PilotMode::executeRotate (Command cmd) {
  float wantedRotation = (cmd.getParams().toFloat());
  lookout->showCommand(STR_COMMAND_ROTATE);
  movement->rotate(wantedRotation);
  commander->sendResult(Success);
}

void PilotMode::executeGoForward (Command cmd) {
  int wantedDistance = -1;
  float wantedSpeed = 0.0;
  bool allowAlternateMovement = true;
  String remaining = cmd.getParams();
  while (remaining.length() > 0) {
    String thisPortion = remaining;
    if (remaining.indexOf(COMMAND_PARAM_DELIM) != -1) {
      thisPortion = remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM));
      remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);
    }
    else {
      remaining = "";
    }

    if (wantedDistance == -1) {
      wantedDistance = thisPortion.toInt();
    }
    else if (wantedSpeed == 0.0) {
      wantedSpeed = thisPortion.toFloat();
    }
    else {
      allowAlternateMovement = thisPortion.equalsIgnoreCase("true");
    }
  }
  lookout->showCommand(STR_COMMAND_GO_FORWARD);

  bool result = driveForward(wantedDistance, wantedSpeed, allowAlternateMovement);
  commander->sendResult(result ? Success : Fail);
}

void PilotMode::executeGoReverse (Command cmd) {
  lookout->showCommand(STR_COMMAND_GO_REVERSE);

  int wantedDistance = -1;
  float wantedSpeed = 0.0;
  bool allowAlternateMovement = true;
  String remaining = cmd.getParams();
  while (remaining.length() > 0) {
    String thisPortion = remaining;
    if (remaining.indexOf(COMMAND_PARAM_DELIM) != -1) {
      thisPortion = remaining.substring(0, remaining.indexOf(COMMAND_PARAM_DELIM));
      remaining = remaining.substring(remaining.indexOf(COMMAND_PARAM_DELIM)+1);
    }
    else {
      remaining = "";
    }

    if (wantedDistance == -1) {
      wantedDistance = thisPortion.toInt();
    }
    else if (wantedSpeed == 0.0) {
      wantedSpeed = thisPortion.toFloat();
    }
    else {
      allowAlternateMovement = thisPortion.equalsIgnoreCase("true");
    }
  }

  bool result = driveBackward(wantedDistance, wantedSpeed, allowAlternateMovement);
  commander->sendResult(result ? Success : Fail);
}

void PilotMode::executeStrafe (Command cmd) {
  String wantedDirection = (cmd.getParams().substring(0, cmd.getParams().indexOf(COMMAND_PARAM_DELIM)));
  int wantedDuration = ((cmd.getParams().substring(cmd.getParams().indexOf(COMMAND_PARAM_DELIM)+1))).toInt();
  commander->log(STR_STATUS_STRAFING);
  lookout->showCommand(STR_COMMAND_STRAFE);
  

  StrafeDirection sd = directionForStrafeName(wantedDirection);
  if (sd == UNKNOWN_DIRECTION) {
    commander->log(STR_DIRECTION_UNKNOWN);
    commander->sendResult(Fail);
    return;
  }

  bool result = strafe(sd, wantedDuration);
  commander->sendResult(result ? Success : Fail);
}

void PilotMode::executeStop (Command cmd) {
  movement->stop();
  lookout->showCommand(STR_COMMAND_STOP);
  commander->log(STR_STATUS_STOPPING);
  commander->sendResult(Success);
}
