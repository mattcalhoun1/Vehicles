#ifndef SBGLOBALS_H

#define SBGLOBALS_H

#define LOG_ENABLED true

// X axis stepper
#define STEPPER_X_SLOW_SPEED 100
#define STEPPER_X_MEDIUM_SPEED 300
#define STEPPER_X_FAST_SPEED 500
#define STEPPER_X_STEPS_PER_REV 6500
#define STEPPER_X_MM_PER_REV 2.3//3.125

#define STEPPER_BIG_STEP_SIZE_SCALED 10 // x's to move before checking for checking state
#define STEPPER_SMALL_STEP_SIZE_SCALED 1 // x's to move when precision is more important (like calibration)

#define AXIS_SCALER_X 1.0 // 1 x = 1 mm
#define AXIS_NAME_X "X"

#define STEPPER_X_LOWER_LIMIT_PIN 3
#define STEPPER_X_UPPER_LIMIT_PIN 2
#define STEPPER_X_DIRECTION_PIN 9
#define STEPPER_X_PULSE_PIN 8

#endif