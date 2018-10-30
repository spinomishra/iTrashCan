#pragma once

enum DriveState {
	Stop = 1,	// default state
	GoForward,
	TurnLeft,
	TurnRight,
	GoBack,
	Continue
};

struct TrashCanData {
	Color color;
	float obstructionDistance;
	int time;
	bool goingBack;
};