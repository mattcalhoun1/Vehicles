#include "CameraServos.h"
#include "Camera.h"
#include "Display.h"
#include "Display_1331.h"
#include "ControlMode.h"
#include "TestMode.h"
#include "WatchMode.h"
#include "LoRaTrans.h"

CameraServos* servos;
Camera* camera;
Display* display;
ControlMode* controlMode;
LoRaTrans* lora;

void setup() {
  //while (!Serial) delay(10);
  Serial.begin(115200);
  delay(100);

  int startingTilts[2] = {90,90}; // vertical
  int startingRotations[2] = {90,90}; // forward
  servos = new CameraServos();
  servos->init(startingRotations, startingTilts);
  logConsole("Servos initialized.");

  display = new Display_1331();
  camera = new Camera();
  lora = new LoRaTrans();

  controlMode = new WatchMode (display, camera, servos, lora);
  controlMode->init();
}

void loop() {
  controlMode->loop();
}

void logConsole (String msg) {
  Serial.println(msg);
}