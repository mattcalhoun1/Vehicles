#ifndef MCGLOBALS_H

#define MCGLOBALS_H

#define LOG_ENABLED false

#define STR_MODE_RANDOM "Random"
#define STR_MODE_PILOT "Pilot"
#define STR_MODE_TEST "Test"

#define US_OBSTRUCTION_THRESHOLD 30 //cm from undercar sensors
#define LD_OBSTRUCTION_THRESHOLD 500.0 //mm == ~ 20 inches

#define LD_FULL_CYCLE_TIME 6000 // how long to allow lidar to refill measurement array / drop old measurements

#define ACTIVE_MOVEMENT_CYCLE_TIME 200 // break forward/reverse movements into chunks of this size for good measurements
#define ACTIVE_STRAFE_CYCLE_TIME 2000 // break strafe movements into chunks of this size for good measurements

#define STR_STATUS_MEASURING "Measuring"
#define STR_STATUS_STRAFE_R "Strafe R"
#define STR_STATUS_STRAFE_L "Strafe L"
#define STR_STATUS_GO_FORWARD "Go Forward"
#define STR_STATUS_GO_REVERSE "Go Reverse"
#define STR_STATUS_OBSTRUCTION_REAR "Rear Obst"
#define STR_STATUS_OBSTRUCTION_FRONT "Front Obst"
#define STR_STATUS_STRAFING "Strafing"
#define STR_STATUS_STOPPING "Stopping"
#define STR_STATUS_BACKED_OUT "Backed Out"
#define STR_STATUS_STOPPED "Stopped"
#define STR_STATUS_ROTATE_L "Rotate L"
#define STR_STATUS_ROTATE_R "Rotate R"

#define STR_STATUS_OBSTRUCTION_US "US Obst"
#define STR_STATUS_OBSTRUCTION_LD "LD Obst"

#define STR_FL "FL"
#define STR_FR "FR"
#define STR_RL "RL"
#define STR_RR "RR"

#define STR_DIRECTION_LF "LF"
#define STR_DIRECTION_LL "LL"
#define STR_DIRECTION_LB "LB"
#define STR_DIRECTION_RF "RF"
#define STR_DIRECTION_RR "RR"
#define STR_DIRECTION_RB "RB"
#define STR_DIRECTION_UNKNOWN "UNKNOWN"

#endif