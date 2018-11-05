#pragma once

#include "ColorDefinitions.h"

#define OUTOFRANGE 401
#define SAFEDISTANCE 7
#define PASSTHRUDISTANCE 50
#define NORMALSPEED_LEFT 129
#define NORMALSPEED_RIGHT 150
#define SLOW_WHEEL_POWER  110
#define USS_SERVO_BASE_ANGLE 80

enum DriveState {
	Stop = 1,	// default state
	GoForward,
	TurnLeft,
	TurnRight,
	GoBack,
	Continue
};

struct ControlData {
	Color color;
	float obstructionDistance;
	int time;
	bool goingBack;
	bool moving;
};