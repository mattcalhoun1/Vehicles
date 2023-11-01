#ifndef MCGLOBALS_H

#define MCGLOBALS_H

#define LOG_ENABLED true

#define STR_MODE_RANDOM "Random"
#define STR_MODE_PILOT "Pilot"
#define STR_MODE_TEST "Test"

#define LD_OBSTRUCTION_THRESHOLD 500.0 //mm == ~ 20 inches

#define LD_FULL_CYCLE_TIME 6000 // how long to allow lidar to refill measurement array / drop old measurements

#define STR_STATUS_MEASURING "Measuring"
#define STR_STATUS_STRAFE_R "Strafe R"
#define STR_STATUS_STRAFE_L "Strafe L"
#define STR_STATUS_GO_FORWARD "Go Forward"
#define STR_STATUS_GO_REVERSE "Go Reverse"
#define STR_STATUS_OBSTRUCTION_REAR "Rear Obst"
#define STR_STATUS_OBSTRUCTION_FRONT "Front Obst"

#define STR_STATUS_OBSTRUCTION_LD "LD Obst"

#endif