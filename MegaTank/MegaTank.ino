#include <Arduino.h>
#include <Wire.h>
#include "Obstructions.h"
#include "VehicleMovement.h"
#include "VehicleServos.h"
#include "Commander.h"

#define MAX_SUPPORTED_CAMS 2 // controls how many cameras can be controlled via commander

VehicleMovement movement;
VehicleServos servos;
Obstructions* obstructions = new Obstructions();
int ultrasonicObstructionThreshold = 35; //cm from undercar sensors
float lidarObstructionThreshold = 500.0; //mm == ~ 20 inches
Commander commander;

int minRotations[2] = {-140,0};
int maxRotations[2] = {0,140};
bool directionsForward[2] = {false,true};
unsigned long lastLog = 0;
bool runLidar = true; 

void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial2.begin(9600);
  Serial2.flush();

  commander.log("Starting...");
  // Attach interrupts, unfortunately this can only be done in this sketch, not in the class
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(LEFT_FRONT)), notifyMovementLF, HIGH);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(RIGHT_FRONT)), notifyMovementRF, HIGH);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(LEFT_REAR)), notifyMovementLR, HIGH);
  attachInterrupt(digitalPinToInterrupt(movement.getEncoderAPin(RIGHT_REAR)), notifyMovementRR, HIGH);

  int startingRotations[2] = {0,0}; // forward
  int startingTilts[2] = {95,95}; // vertical
  servos.init(startingRotations, startingTilts);

  obstructions->init();
  obstructions->startLidar();
}

// Invoke the appropriate interrupt to increment pulse counts
void notifyMovementLF () {
  movement.interrupt(LEFT_FRONT);
}

void notifyMovementRF () {
  movement.interrupt(RIGHT_FRONT);
}

void notifyMovementLR () {
  movement.interrupt(LEFT_REAR);
}

void notifyMovementRR () {
  movement.interrupt(RIGHT_REAR);
}


bool isObstructed () {
  int leftBumperDist = obstructions->getDistanceBumperLeft();
  int rightBumperDist = obstructions->getDistanceBumperRight();
  bool usLeftObstructed = leftBumperDist <= ultrasonicObstructionThreshold;
  bool usRightObstructed = obstructions->getDistanceBumperRight() <= ultrasonicObstructionThreshold;
  
  /*if (usLeftObstructed) {
    commander.log("Ultrasonic shows obstruction near left bumper at distance " + String(leftBumperDist) + " cm");
  }
  if (usRightObstructed) {
    commander.log("Ultrasonic shows obstruction near right bumper at distance " + String(rightBumperDist) + " cm");
  }*/
  Measurement closestLidarObst = obstructions->getClosestObstruction(360.0 - 25.0, 25.0, lidarObstructionThreshold);
  bool lidarObstructed = closestLidarObst.distance > -1.0 && closestLidarObst.distance <= lidarObstructionThreshold;

  //if (lidarObstructed) {
  //  commander.log("LIDAR shows front obstruction at " + String(closestLidarObst.angle) + " degrees, distance of " + String(closestLidarObst.distance) + " mm");
  //}

  return usLeftObstructed || usLeftObstructed || lidarObstructed;
  
}

bool isRearObstructed () {
  Measurement closestLidarObst = obstructions->getClosestObstruction(180.0 - 25.0, 180.0 + 25.0, lidarObstructionThreshold);
  bool lidarObstructed = closestLidarObst.distance > -1.0 && closestLidarObst.distance <= lidarObstructionThreshold;

  /*if (lidarObstructed) {
    commander.log("LIDAR shows rear obstruction at " + String(closestLidarObst.angle) + " degrees, distance of " + String(closestLidarObst.distance) + " mm");
  }*/

  return lidarObstructed;
}

void sendLidarMap (Command cmd) {
  float* lidarMap = obstructions->getDistances();
  commander.sendResult(Success);
  commander.sendMap(obstructions->getLidarHeading(), obstructions->getDistances(), obstructions->getNumMeasurements());
}

void sendConfig (Command cmd) {
  if(cmd.getParams().equalsIgnoreCase("LidarHeading")) {
    commander.sendResult(Success);
    commander.sendConfig("LidarHeading", obstructions->getLidarHeading());
  }
  else if(cmd.getParams().equalsIgnoreCase("LidarNumMeasurements")) {
    commander.sendResult(Success);
    commander.sendConfig("LidarNumMeasurements", obstructions->getNumMeasurements());
  }
  else if(cmd.getParams().equalsIgnoreCase("LidarGranularity")) {
    commander.sendResult(Success);
    commander.sendConfig("LidarGranularity", obstructions->getMeasurementsIncrement());
  }
  else {
    commander.sendResult(Fail);
  }
}

void collectMeasurement(Command cmd) {
  // The param should contain angle we want
  float wantedAngle = (cmd.getParams().substring(0, cmd.getParams().indexOf("|"))).toFloat();
  float wantedTolerance = ((cmd.getParams().substring(cmd.getParams().indexOf("|")+1))).toFloat();
  commander.log("Collecting measurement at angle " + String(wantedAngle) + " within tolerance of degrees " + String(wantedTolerance));

  Measurement meas = obstructions->getDistance(wantedAngle, wantedTolerance, 10000);
  commander.log("Lidar distance at " + String(meas.angle) + " degrees == " + String(meas.distance) + " mm");
  commander.sendResult(Success);
  commander.sendMeasurement(meas.angle, meas.distance);
}

void look (Command cmd) {
  int wantedRotation = (cmd.getParams().substring(0, cmd.getParams().indexOf("|"))).toInt();
  int wantedTilt = ((cmd.getParams().substring(cmd.getParams().indexOf("|")+1))).toInt();

  int camNum = servos.getCameraFor(wantedRotation, wantedTilt);
  servos.setCameraRotation(wantedRotation, camNum);
  servos.setCameraTilt(wantedTilt, camNum);
  commander.sendResult(Success);
  commander.log("Camera " + String(camNum) + " adjusted");
}

void sendCameraPositions (Command cmd) {
  // if number of cameras is increased, this fixed array size must increase
  int rotations[MAX_SUPPORTED_CAMS];
  int tilts[MAX_SUPPORTED_CAMS];
  int minRotations[MAX_SUPPORTED_CAMS];
  int maxRotations[MAX_SUPPORTED_CAMS];
  int minTilts[MAX_SUPPORTED_CAMS];
  int maxTilts[MAX_SUPPORTED_CAMS];

  for (int camCount = 0; camCount < servos.getNumCameras() && camCount < MAX_SUPPORTED_CAMS; camCount++) {
    rotations[camCount] = servos.getVehicleRelativeRotation(camCount);
    tilts[camCount] = servos.getCameraTilt(camCount);
    minRotations[camCount] = servos.getMinVehicleRelativeRotation(camCount);
    maxRotations[camCount] = servos.getMaxVehicleRelativeRotation(camCount);
    minTilts[camCount] = servos.getMinTilt(camCount);
    maxTilts[camCount] = servos.getMaxTilt(camCount);
  }

  commander.sendResult(Success);
  commander.sendCameras (rotations, tilts, minRotations, maxRotations, minTilts, maxTilts, servos.getNumCameras());
}

void executeGoForward (Command cmd) {
  int wantedDistance = (cmd.getParams().substring(0, cmd.getParams().indexOf("|"))).toInt();
  float wantedSpeed = ((cmd.getParams().substring(cmd.getParams().indexOf("|")+1))).toFloat();
  commander.log("Driving forward " + String(wantedDistance) + " units at speed " + String(wantedSpeed));

  bool result = driveForward(wantedDistance, wantedSpeed);
  commander.sendResult(result ? Success : Fail);
}

void executeGoReverse (Command cmd) {
  int wantedDistance = (cmd.getParams().substring(0, cmd.getParams().indexOf("|"))).toInt();
  float wantedSpeed = ((cmd.getParams().substring(cmd.getParams().indexOf("|")+1))).toFloat();
  commander.log("Driving in reverse " + String(wantedDistance) + " units at speed " + String(wantedSpeed));

  bool result = driveBackward(wantedDistance, wantedSpeed);
  commander.sendResult(result ? Success : Fail);
}

void executeStop (Command cmd) {
  commander.log("Stopping");
  movement.stop();
  commander.sendResult(Success);
}

void loop () {
  waitLoop();
  //driveLoop();
}

void backOut () {
  float totalRotated = 0.0;

  // We should already be stopped
  // check for rear obstructions
  while (isRearObstructed()) {
    // Rotate a little
    commander.log("Want to back up, but rear is obstructed. rotating.");
    movement.rotate(75.0);
    totalRotated += 75.0;

    sleepOrBackground(6000); // give time to update lidar
  }
  commander.log("Rear is now unobstructed");

  if (driveBackward(12, 20.0)) {
    movement.stop();
  }
}

float getRandomRotation () {
  while(true) {
    float randomRotation = 180.0 - float(random(360));
    if (float(abs(randomRotation)) > 30.0) {
      return randomRotation;
    }
  }
}

void rotateRandomly() {
    bool foundGoodAngle = false;
    while (!foundGoodAngle) {
      float randomRotation = getRandomRotation();
      commander.log("Random rotation " + String(randomRotation));

      // Check for obstructions at that angle. need at least 6 feet of clearance
      Measurement nearObstruction = obstructions->getClosestObstruction(randomRotation - 20.0, randomRotation + 20.0, 900);
      Measurement farObstruction = obstructions->getClosestObstruction(randomRotation - 10.0, randomRotation + 10.0, 1800);
      if (nearObstruction.distance < 0.0 && farObstruction.distance < 0.0) {
        foundGoodAngle = true;
        commander.log("Path looks clear at " + String(randomRotation) + " degrees, rotating");
        movement.rotate(randomRotation);
      }
    }

}

bool driveForward (int distance, float speed) {
  // needs a timeout
  commander.log("Going Forward");
  movement.resetPulses();
  movement.resetTripMeter();

  movement.setDirectionSpeed(true, speed);
  unsigned long sleepCycle = 100;
  bool allTheWay = true;
  while (movement.getDistanceTraveled() < distance) {
    sleepOrBackground(sleepCycle);
    movement.updateSpeeds();

    // Check for obstuctions
    if (isObstructed()) {
      movement.stop();
      allTheWay = false;
      //backOut();
      break;
    }
  }

  return allTheWay;
}

bool driveBackward (int distance, float speed) {
  // needs a timeout
  commander.log("Going Backward");
  movement.resetPulses();
  movement.resetTripMeter();

  movement.setDirectionSpeed(false, speed);
  unsigned long sleepCycle = 100;
  bool allTheWay = true;
  while (movement.getDistanceTraveled() < distance) {
    sleepOrBackground(sleepCycle);
    movement.updateSpeeds();

    // Check for obstuctions
    if (isRearObstructed()) {
      movement.stop();
      allTheWay = false;
      break;
    }
  }

  return allTheWay;
}

void driveLoop() {
  commander.sendPing();

  servos.setVehicleRelativeRotation(0.0, 0);
  servos.setVehicleRelativeRotation(0.0, 1);
  servos.setCameraTilt(90.0, 0);

  rotateRandomly();

  movement.resetPulses();

  // make a random turn, as long as the path is not obstructed
  if (driveForward(36, 20.0)) {
    // Stop
    movement.stop();
    commander.log("Stopped");
  }
  else {
    backOut();
    commander.log("Trip was cut short, backed out.");
  }

  // allow time for lidar to catch up
  sleepOrBackground(4000);
}

// when unsure what to do, this is what to do
void waitLoop () {

  // make sure vehicle is stopped
  movement.stop();
  commander.sendPing();

  Command nextCommand = commander.getNextCommand();
  switch (nextCommand.getCommandType()) {
    case Measure:
      collectMeasurement(nextCommand);
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
  }
  /*if (nextCommand.getCommandType() != Nothing) {
    if (nextCommand.getCommandType() == Measure) {
      collectMeasurement(nextCommand);
    }
    else if (nextCommand.getCommandType() == GetConfig) {
      sendConfig(nextCommand);
    }
    else if (nextCommand.getCommandType() == LidarMap) {
      sendLidarMap(nextCommand);
    }
    else if (nextCommand.getCommandType() == GetCameras) {
      sendCameraPositions(nextCommand);
    }
    else if (nextCommand.getCommandType() == Forward) {
      executeForward(nextCommand);
    }
    else if (nextCommand.getCommandType() == Reverse) {
      executeReverse(nextCommand);
    }
    else if (nextCommand.getCommandType() == Stop) {
      executeStop(nextCommand);
    }
  }*/
  sleepOrBackground(1000);
}

void testLoop() {
  bool testMovement = false;
  bool testServos = true;
  bool testLidar = true;
  commander.sendPing();

  // Check for a command
  /*
  Command nextCommand = commander.getNextCommand();
  if (nextCommand.getCommandType() != Nothing) {
    if (nextCommand.getCommandType() == Measure) {
      collectMeasurement(nextCommand);
    }
  }*/


  if (testServos) {
    for (int cam = 0; cam < 2; cam++) {
      commander.log("getting vehicle rotation for cam " + String(cam));
      int nextAngle = servos.getVehicleRelativeRotation(cam);
      if (directionsForward[cam]) {
        nextAngle += 25;
        if (nextAngle > maxRotations[cam]) {
          nextAngle = maxRotations[cam] - 25;
          directionsForward[cam] = !directionsForward[cam];
        }
      }
      else {
        nextAngle -= 25;
        if (nextAngle < minRotations[cam]) {
          nextAngle = minRotations[cam] + 25;
          directionsForward[cam] = !directionsForward[cam];
        }
      }
      commander.log("next rotation for cam " + String(cam) + " is " + String(nextAngle));
      servos.setVehicleRelativeRotation(nextAngle, cam);
      sleepOrBackground(1000);
    }
  }

  if (testLidar) {
    commander.log("Testing lidar");
    //obstructions.collectMeasurements(2000);
    float lidarCheck = 90.5;
    Measurement meas = obstructions->getDistance(lidarCheck, 5.0, 10000);
    commander.log("Lidar distance at " + String(meas.angle) + " degrees == " + String(meas.distance) + " mm");

    // Expecting wall at about 147 inches (3733 mm)
    Measurement matchedDist = obstructions->getDistanceInRange(lidarCheck, 2.0, 3733.0, 500.0, 10000);
    if (matchedDist.angle >= 0) {
      commander.log("Matched distance at " + String(meas.angle) + " degrees == " + String(meas.distance) + " mm");
    }
    else {
      commander.log("No match found for the expected distance");
    }
  }

  if (testMovement) {
    commander.log("Resetting pulses");
    movement.resetPulses();

    // Set the direction forward
    commander.log("Going Forward");
    movement.setDirectionSpeed(true, 15.0);
    for (int step = 0; step < 3; step++) {
      sleepOrBackground(1000);
      movement.updateSpeeds();
    }

    // Stop
    movement.stop();
    commander.log("Stopped");
    sleepOrBackground(3000);

    // Check the distance
    commander.log("Total Distance So Far: " + String(movement.getDistanceTraveled()));

    commander.log("Going Backward");
    movement.setDirectionSpeed(false, 15.0);
    for (int step = 0; step < 3; step++) {
      sleepOrBackground(1000);
      movement.updateSpeeds();
    }

    // Stop
    movement.stop();

    // Check the distance
    commander.log("Total Distance So Far: " + String(movement.getDistanceTraveled()));

    // Rotate -90
    commander.log("Rotating left");
    movement.rotate(-90.0);

    sleepOrBackground(1000);

    // rotate +90
    commander.log("Rotating right");
    movement.rotate(90.0);
  }

  commander.log("Test loop complete, sleeping for 2 sec");
  sleepOrBackground(2000);
  commander.log("Sleep done");
}

void sleepOrBackground(unsigned long sleepTime) {
  if (runLidar) {
    // collect measurements for sleep time
    obstructions->collectMeasurements(sleepTime);
  }
  else {
    // do nothing for sleep time
    delay(sleepTime);
  }
}
