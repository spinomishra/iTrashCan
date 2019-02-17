//
// ©Copyright 2018, All Rights Reserved.
//
// iTrashCan_Definitions.h created by spinomishra on 10/29/2018T10:43 PM
//
#pragma once

#include "ColorDefinitions.h"

// Normal motor pwms for left and right wheels
#define NORMALSPEED_LEFT 60
#define NORMALSPEED_RIGHT 50

// Motors PWMs for reversing 
#define REVERSESPEED_LEFT 70	/*90*/
#define REVERSESPEED_RIGHT 60	/*80*/

// Ultrasonic ranges (cm)
#define OUTOFRANGE 401
#define SAFEDISTANCE 45	
#define PASSTHRUDISTANCE 50

#define SLOW_WHEEL_POWER  40
#define USS_SERVO_BASE_ANGLE 80
#define SIMULATION_WAIT_TIME  15	
#define MILLISECONDS	1000

struct ControlData {
	Color color;
	float obstructionDistance;
	int time;
	bool goingBack;
	bool moving;
};

enum Markers {
	Clear,
	RedMarker,
	GreenMarker,
	BlueMarker,	
};