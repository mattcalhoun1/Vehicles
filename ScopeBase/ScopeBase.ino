#include <Stepper.h>

//const int stepsPerRevolution = 32767;
const int stepsPerRevolution = 6500;
float mmPerRevolution = 3.125;
int bigStepSizeScaled = 10; // x's to move in one shot

float scaledXmin = 0;
float scaledXmax = 100.0;
float xScaler = 1.0;// mm per x

int limitSwitchPort = 49;

Stepper myStepper(stepsPerRevolution, 8, 9);

void setup() {
  Serial.begin(9600);

  writeLog("attaching interrupts");
  pinMode(limitSwitchPort, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(limitSwitchPort), limitFound, LOW);
  writeLog("interrupts attached");
  //attachInterrupt(digitalPinToInterrupt(limitSwitchPort), limitFound, FALLING);

  myStepper.setSpeed(500); //speed is in RPM;
}

void loop() {
  float xPosition = 50.0;

  //slideX (true, 40);

  Serial.println("Small move"); //controls the direction of the stepper motor;
  slideX (true, 10.0);
  delay(500);
  slideX (false, 10.0);
  delay(500);

  Serial.println("medium move"); //controls the direction of the stepper motor;
  slideX (true, 20.0);
  delay(500);
  slideX (false, 20.0);
  delay(500);
}

// scaled distance is somewhere between xmin and xmax (0 and 100)
void slideX (bool forward, float scaledDistance) {
  int scaledSteps = scaledDistance / bigStepSizeScaled;
  float lastScaledStep = scaledDistance - (scaledSteps * bigStepSizeScaled);
  int directionMultiplier = forward ? 1 : -1;

  writeLog("Scaled steps: " + String(scaledSteps) + ", last scaled step: " + String(lastScaledStep));

  for (int stepCount = 0; stepCount < scaledSteps; stepCount++) {
    myStepper.step(directionMultiplier * stepsPerRevolution * bigStepSizeScaled);
  }

  if (lastScaledStep != 0) {
    myStepper.step(directionMultiplier * lastScaledStep * bigStepSizeScaled * bigStepSizeScaled);
  }
}

void limitFound() {
  writeLog("limit found");  
}

void writeLog (String msg) {
  Serial.println(msg);
  //delay(500);
}