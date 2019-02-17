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

	// this will block execution if color sensor is not found
	DetectColorSensor();

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

	// No power to any motors
	StopMovement(0);

	Color color;
	ReadColorSensor(color);
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

void Log(Trashcart_States state)
{
	switch (state)
	{
	case State_StoppedAtBase:Serial.print(" At Base "); break;
	case State_DrivingToPickupPoint:Serial.print(" Driving to Pickup Point "); break;
	case State_ReachedPickupPoint:Serial.print(" At Pickup Point "); break;
	case State_ReturningToBase:Serial.print(" Returning to Base "); break;
	}
}

void Log(DriveEvent drivEvent)
{
	switch (drivEvent)
	{
	case DriveEvent_None:	Serial.print(" None "); break;
	case DriveEvent_Stop:  	Serial.print(" Stop "); break;
	case DriveEvent_GoForward: Serial.print(" go forward "); break;
	case DriveEvent_Trigger:Serial.print(" Timer Triggered "); break;
	case DriveEvent_CrossedLeftBoundary:Serial.print(" crossed left boundary "); break;
	case DriveEvent_CrossedRightBoundary:Serial.print(" crossed right boundary "); break;
	case DriveEvent_FoundObstruction: Serial.print(" found obstruction "); break;
	}
}

void Log(Trashcart_States current, Trashcart_States next, DriveEvent drivingEvent, char* movementDirection, bool obstruction)
{
	Log(current); Serial.print(", ");  
	Log(next); Serial.print(", "); 
	Log(drivingEvent); Serial.print(", ");
	
	if (movementDirection != NULL)
		Serial.print(movementDirection); 
	Serial.print(", ");
	Serial.println(obstruction ? "true" : "false");
}

void loop()
{
	//if (Serial) {
	//	// If serial port is ready
	//	if (Serial.available() > 0)
	//		PerformCommandBasedTests();
	//}
	//else
	{
		DriveEvent drivingEvent = DriveEvent::DriveEvent_None;
		int moveForwardRuntime = 0;

		// first read color data
		ReadColorSensor(controlData.color);
		Trashcart_States nextState = WhatIsNextState(currentState, controlData.color, drivingEvent);

		switch (nextState)
		{
		case Trashcart_States::State_StoppedAtBase:
			if (currentState == State_ReturningToBase)
			{
				StopMovement(100);
				ReverseInPosition();

				Log(currentState, nextState, DriveEvent_None, "ReverseInPosition", false);
			}
			break;

		case Trashcart_States::State_DrivingToPickupPoint:
			if (!IsPathClear())
			{
				drivingEvent = DriveEvent_FoundObstruction;
				Log(currentState, nextState, drivingEvent, "Forward", true);
			}
			else
			{
				if (currentState == State_StoppedAtBase)
					moveForwardRuntime = 1000;
				else
					moveForwardRuntime = 0;
				Log(currentState, nextState, drivingEvent, "Forward", false);
			}
			break;

		case Trashcart_States::State_ReachedPickupPoint:
		{
			StopMovement(500);
			Log(currentState, nextState, drivingEvent, "WaitForPickup", false);
		}
		break;

		case Trashcart_States::State_ReturningToBase:
			if (currentState == State_ReachedPickupPoint)
			{
				ReverseInPosition();
				
				Log(currentState, nextState, drivingEvent, "ReverseInPosition", false);

				Color color;
				Markers marker;
				
				// first move in front of pickup point marker
				while (1)
				{
					ReadColorSensor(color);
					marker = DetermineMarker(color);
					if (marker == RedMarker)
					{
						Serial.println("Moving in front of pickup point marker");
						MoveForward();
						delay(500);
						StopMovement(0);
					}
					else
						break;
				}
			}
			else if (!IsPathClear())
			{
				drivingEvent = DriveEvent_FoundObstruction;
				Log(currentState, nextState, drivingEvent, "Forward", true);
			}
			break;
		}

		switch (drivingEvent)
		{
		case DriveEvent_None:	// continue what ever is currently happening
			break;

		case DriveEvent_Stop:
			StopMovement(500);
			break;

		case DriveEvent::DriveEvent_FoundObstruction:
			AvoidObstruction();
			break;

		case DriveEvent_GoForward:
			MoveForward();

			if (moveForwardRuntime > 0)
				delay(moveForwardRuntime);
			break;

		case DriveEvent_CrossedLeftBoundary:
			StopMovement(100);
			//BringItBackWithinBoundary(DriveEvent_CrossedLeftBoundary);
			Reverse();
			delay(1000);
			StopMovement(550);
			RightTurn(80);
			delay(2000);
			MoveForward();
			delay(500);
			break;

		case DriveEvent_CrossedRightBoundary:
			StopMovement(100);
			//BringItBackWithinBoundary(DriveEvent_CrossedLeftBoundary);
			Reverse();
			delay(1000);
			StopMovement(250);

			LeftTurn(80);
			delay(2000);
			MoveForward();
			delay(500);
			break;
		}

		currentState = nextState;
		Serial.println();
		Serial.println();
	}
}

void Backup()
{
	// first stop
	StopMovement(50);

	// Reverse
	Reverse();
	delay(2000);

	// stop again
	StopMovement(100);
}

bool TimeForTrashToBePicked()
{
	unsigned long waitTime = 0;
#ifdef SIMULATE_WAIT_TIME
	waitTime = SIMULATION_WAIT_TIME * MILLISECONDS;
#else
	waitTime = (7*24 - 6) * 60 * 60 * MILLISECONDS ;	// 7 days
#endif

	Serial.print("Waiting for "); Serial.print(waitTime); Serial.println("ms");
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

void PerformCommandBasedTests()
{
	// read the incoming byte:
	char inChar = (char)Serial.read(); // Read a character

	switch (inChar)
	{
	case 'C': ReadColorSensor(controlData.color); break;
	case 'B': ReverseInPosition(); break;
	case 'L': LeftTurn(80); break;
	case 'R': RightTurn(80); break;
	case 'P': PivotLeft(); break;
	case 'Q': PivotRight(); break;
	case 'S': StopMovement(0); break;
	case 'D': Reverse(); delay(500);  break;
	case 'E': MoveForward(); delay(500); break;
	}

	StopMovement(20);
}
