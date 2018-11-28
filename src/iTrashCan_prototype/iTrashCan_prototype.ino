//
// ©Copyright 2018, All Rights Reserved.
//
// iTrashCan_prototype.ino created by spinomishra on 10/6/2018T10:11 AM
//

#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <time.h>

#include "ColorDefinitions.h"
#include "iTrashCan_Definitions.h"
#include "PinDefinitions.h"
#include "States.h"

// path direction is initially set to stop
ControlData controlData;

#define SIMULATE_WAIT_TIME	1

Trashcart_States currentState = Trashcart_States::State_StoppedAtBase;

bool ReturningToBase = false;


/* output input, true false*/
void setup()
{
	Serial.begin(9600);

	DetectColorSensor();	// this will block execution if color sensor is not found

							// initializing control data
	controlData.moving = false;
	controlData.goingBack = false;
	controlData.obstructionDistance = 0;
	controlData.time = 0;

	// setting up motor input and output pins
	pinMode(leftMotorEnablePin, OUTPUT);
	pinMode(leftMotorInput1, OUTPUT);
	pinMode(leftMotorInput2, OUTPUT);

	pinMode(rightMotorEnablePin, OUTPUT);
	pinMode(rightMotorInput1, OUTPUT);
	pinMode(rightMotorInput2, OUTPUT);

	EnableObstructionDetection();
	DisableObstructionDetection();

	StopMovement();
}

Trashcart_States WhatIsNextState(Trashcart_States currentCardState, Color color, DriveEvent& driveEvent)
{
	Trashcart_States nextState = currentCardState;
	driveEvent = DriveEvent_None;

	if (currentCardState == State_StoppedAtBase || currentCardState == State_ReachedPickupPoint)
	{
		// timer triggered
		if (currentCardState == State_StoppedAtBase)
		{
			if (TimeForTrashToBePicked() == true)
			{
				nextState = State_DrivingToPickupPoint;
				driveEvent = DriveEvent_GoForward;
			}
		}
		else if (TimeForReturning())
		{
			nextState = State_ReturningToBase;
			driveEvent = DriveEvent_GoForward;
		}
	}

	Markers marker = DetermineMarker(color);
	if (marker == RedMarker)
	{
		if (currentState == State_DrivingToPickupPoint)
		{
			nextState = State_ReachedPickupPoint;
			driveEvent = DriveEvent_Stop;
		}

		if (currentState == State_ReturningToBase)
		{
			nextState = State_StoppedAtBase;
			driveEvent = DriveEvent_Stop;
		}
	}

	if (marker == GreenMarker)
	{
		if (nextState == State_DrivingToPickupPoint)
			driveEvent = DriveEvent_CrossedRightBoundary;
		else if (nextState == State_ReturningToBase)
			driveEvent = DriveEvent_CrossedLeftBoundary;
	}

	if (marker == BlueMarker)
	{
		if (nextState == State_DrivingToPickupPoint)
			driveEvent = DriveEvent_CrossedLeftBoundary;
		else if (nextState == State_ReturningToBase)
			driveEvent = DriveEvent_CrossedRightBoundary;
	}

	if (marker == Clear)
	{
		driveEvent = DriveEvent_GoForward;
	}

	return nextState;
}

bool IsPathClear()
{
	// read ultra sonic data
	controlData.obstructionDistance = DetectObstructionDistance();

	// collect all sensors data (time, color, ultrasonic)	
	if (controlData.obstructionDistance != 0 && controlData.obstructionDistance <= SAFEDISTANCE)
		return false;

	return true;
}

void loop()
{
	DriveEvent drivingEvent = DriveEvent::DriveEvent_None;

	// first read color data
	ReadColorSensor(controlData.color);
	Trashcart_States nextState = WhatIsNextState(currentState, controlData.color, drivingEvent);
	
	switch (nextState)
	{
		case Trashcart_States::State_StoppedAtBase:
			if (currentState == State_ReturningToBase)
			{
				StopMovement();
				delay(100);
				ReverseInPosition();
			}
			break;

		case Trashcart_States::State_DrivingToPickupPoint:
			if (!IsPathClear())
			{
				Serial.println("Driving to pickup point... Found obstruction");
				drivingEvent = DriveEvent_FoundObstruction;
			}
			else
				Serial.println("Driving to pickup point");

			break;

		case Trashcart_States::State_ReachedPickupPoint:
			{
				Serial.println("Reached pickup point...will wait for pickup");
				StopMovement();
				delay(500);
			}
			break;

		case Trashcart_States::State_ReturningToBase:
			if (currentState == State_ReachedPickupPoint)
			{
				Serial.println("Getting ready to travel to base.. reverse in position");
				Reverse();
				delay(400);

				ReverseInPosition();

				delay(500);

				Color color;
				ReadColorSensor(color);

				Markers marker = DetermineMarker(color);
				do
				{					
					if (marker == RedMarker)
					{
						MoveForward();
						delay(250);
					}

					if (marker == BlueMarker)
					{
						LeftTurn();
						delay(500);
						StopMovement();
						delay(100);
					}

					if (marker == GreenMarker)
					{
						RightTurn();
						delay(500);
						StopMovement();
						delay(100);
					}

					marker = DetermineMarker(color);
				} while (marker != Clear);
			}
			else if (!IsPathClear())
			{
				Serial.println("Driving to base ... Found obstruction");
				drivingEvent = DriveEvent_FoundObstruction;
			}
			break;
	}

	switch (drivingEvent)
	{
		case DriveEvent_None:	// continue what ever is currently happening
			break;

		case DriveEvent_Stop:
			StopMovement();
			delay(500);
			break;

		case DriveEvent::DriveEvent_FoundObstruction:
			AvoidObstruction();
			break;

		case DriveEvent_GoForward:
			MoveForward();
			break;

		case DriveEvent_CrossedLeftBoundary:
			Backup();
			RightTurn();
			delay(350);
			StopMovement();
			delay(100);
			break;

		case DriveEvent_CrossedRightBoundary:
			Backup();
			LeftTurn();
			delay(350);
			StopMovement();
			delay(100);
			break;
	}

	currentState = nextState;	
}

void Backup()
{
	// first stop
	StopMovement();

	// Reverse
	Reverse();
	delay(1000);

	// stop again
	StopMovement();
	delay(100);
}

bool TimeForTrashToBePicked()
{
	unsigned long waitTime = 0;
#ifdef SIMULATE_WAIT_TIME
	waitTime = SIMULATION_WAIT_TIME * MILLISECONDS;
#else
	waitTime = (7*24 - 6) * 60 * 60 * MILLISECONDS ;	// 7 days
#endif

	delay(waitTime);
	return true;
}

bool TimeForReturning()
{
	unsigned long waitTime = 0;

#ifdef SIMULATE_WAIT_TIME
	waitTime = SIMULATION_WAIT_TIME * MILLISECONDS;
#else
	waitTime = 6 * 60 * 60 * MILLISECONDS;	// 6 hours
#endif

	delay(waitTime);
	return true;
}
