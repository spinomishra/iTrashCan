//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 10/29/2018T10:43 PM
//
#pragma once

#include "ColorDefinitions.h"

#define OUTOFRANGE 401
#define SAFEDISTANCE 7
#define PASSTHRUDISTANCE 50
#define NORMALSPEED_LEFT 129
#define NORMALSPEED_RIGHT 150
#define SLOW_WHEEL_POWER  110
#define USS_SERVO_BASE_ANGLE 80
#define SIMULATION_WAIT_TIME  15	
#define MILLISECONDS	1000

enum MovementState {
	Stop = 1,	// default state
	GoForward,
	TurnLeft,
	TurnRight,
	GoBack,
	Obstruction,
	BackupAndTurnRight,
	BackupAndTurnLeft,
	Continue
};

struct ControlData {
	Color color;
	float obstructionDistance;
	int time;
	bool goingBack;
	bool moving;
};