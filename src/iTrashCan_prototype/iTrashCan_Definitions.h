#pragma once

#include "ColorDefinitions.h"

#define OUTOFRANGE 400
#define SAFEDISTANCE 15
#define PASSTHRUDISTANCE 50

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