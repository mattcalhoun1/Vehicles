#include "Lookout.h"
#include <Arduino.h>

void Lookout::init () {
  showStatus("Due OK");
}

// Disable if ONLY lidar is desired for identifying obstructions
bool Lookout::bumperSensorsEnabled() {
  return true;
}

bool Lookout::isCompleteMeasurement(String msg) {
  // should contain DIST:num|num|num|num
  if (msg.substring(0, 4).equalsIgnoreCase("DIST")) {
    int pipeCount = 0;
    for (int charCount = 4; charCount < msg.length(); charCount++) {
      if (msg.charAt(charCount) == '|') {
        pipeCount++;
      }
    }
    return pipeCount == 3;
  }
  return false;
}

String Lookout::readLatestMeasurements () {
  char delim = '\n';
  String newest = "";
  String lastComplete = "";
  unsigned long startTime = millis();

  // Jump to newest measurement
  while (lookoutPort->available() > 2 && (millis() - startTime) < 500) {
    if (newest.length() > 2) {
      lastComplete = newest;
    }
    newest = lookoutPort->readStringUntil(delim);
  }
  
  if (isCompleteMeasurement(newest)) {
    return newest;
  }

  if (isCompleteMeasurement(lastComplete)) {
    return lastComplete;
  }
  return "";
}

void Lookout::refreshDistances() {
  String resp = readLatestMeasurements();
  if (resp.length() > 0) {
    String params = "";
    int delim = resp.indexOf(':');

    if (delim != -1) {
      params = resp.substring(delim+1);
      resp.trim();
      resp = resp.substring(0, delim);
    }
    if (resp.equalsIgnoreCase("DIST")) {
      // The distances will be delimited by pipe
      lastLeft = (params.substring(0, params.indexOf("|"))).toInt();
      String remaining = params.substring(params.indexOf("|")+1);
      lastRight = (remaining.substring(0, remaining.indexOf("|"))).toInt();
      remaining = remaining.substring(remaining.indexOf("|")+1);
      lastRearLeft = (remaining.substring(0, remaining.indexOf("|"))).toInt();
      remaining = remaining.substring(remaining.indexOf("|")+1);
      lastRearRight = (remaining.substring(0, remaining.indexOf("|"))).toInt();
    }
  }
}

// display
void Lookout::showObstruction (String location, int distance) {
  lookoutPort->print("SHOWOBSTRUCTION:" + location + "|" + String(distance));
  lookoutPort->print("\n");
}

void Lookout::showStatus (String status) {
  lookoutPort->print("SHOWSTATUS:" + status);
  lookoutPort->print("\n");
}

void Lookout::showPosition (double x, double y, double heading) {
  lookoutPort->print("SHOWPOSITION:" + String(x) + "|" + String(y) + "|" + String(heading));
  lookoutPort->print("\n");
}

void Lookout::showCommand (String command) {
  lookoutPort->print("SHOWCOMMAND:" + command);
  lookoutPort->print("\n");
}

void Lookout::showMode (String mode) {
  lookoutPort->print("SHOWMODE:" + mode);
  lookoutPort->print("\n");
}

void Lookout::clearDisplay () {
  lookoutPort->print("CLEARDISPLAY:x");
  lookoutPort->print("\n");
}


unsigned long Lookout::getDistanceBumperLeft()
{
  return lastLeft;
}

unsigned long Lookout::getDistanceBumperRight()
{
  return lastRight;
}

unsigned long Lookout::getDistanceRearBumperLeft()
{
  return lastRearLeft;
}

unsigned long Lookout::getDistanceRearBumperRight()
{
  return lastRearRight;
}