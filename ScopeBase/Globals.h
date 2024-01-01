#ifndef SBGLOBALS_H

#define SBGLOBALS_H

#define LOG_ENABLED true

#define STEPPER_BIG_STEP_SIZE_SCALED 10 // x's to move before checking for checking state
#define STEPPER_SMALL_STEP_SIZE_SCALED 1 // x's to move when precision is more important (like calibration)

// X axis stepper
#define STEPPER_X_SLOW_SPEED 100
#define STEPPER_X_MEDIUM_SPEED 250
#define STEPPER_X_FAST_SPEED 350
#define STEPPER_X_STEPS_PER_REV 6500
#define STEPPER_X_MM_PER_REV 2.3//3.125

#define AXIS_SCALER_X 1.0 // 1 x = 1 mm
#define AXIS_NAME_X "X"

#define STEPPER_X_LOWER_LIMIT_PIN 3
#define STEPPER_X_UPPER_LIMIT_PIN 2
#define STEPPER_X_DIRECTION_PIN 9
#define STEPPER_X_PULSE_PIN 8

// Y axis stepper
#define STEPPER_Y_SLOW_SPEED 100
#define STEPPER_Y_MEDIUM_SPEED 250
#define STEPPER_Y_FAST_SPEED 350
#define STEPPER_Y_STEPS_PER_REV 6500
#define STEPPER_Y_MM_PER_REV 2.3//3.125

#define AXIS_SCALER_Y 1.0 // 1 x = 1 mm
#define AXIS_NAME_Y "Y"

#define STEPPER_Y_LOWER_LIMIT_PIN 5
#define STEPPER_Y_UPPER_LIMIT_PIN 4
#define STEPPER_Y_DIRECTION_PIN 11
#define STEPPER_Y_PULSE_PIN 10


#endif