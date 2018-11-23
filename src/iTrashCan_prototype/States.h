//
// ©Copyright 2018, Dell Inc., All Rights Reserved.
// This material is confidential and a trade secret.  Permission to use this
// work for any purpose must be obtained in writing from Dell Inc.
//
// States.h created by spinomishra on 11/04/2018T10:59 AM
//
#pragma once

enum Trashcart_States {
	State_StoppedAtBase,
	State_DrivingToPickupPoint,	
	State_ReachedPickupPoint,
	State_ReturningToBase,
};

enum DriveEvent {
	DriveEvent_None,	// default state
	DriveEvent_Stop,
	DriveEvent_GoForward,	
	DriveEvent_Trigger,
	DriveEvent_CrossedLeftBoundary,
	DriveEvent_CrossedRightBoundary,
	DriveEvent_FoundObstruction
};

enum MoveDirection {
	TurnLeft,
	TurnRight,
	None
};
