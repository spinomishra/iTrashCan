//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 10/6/2018T10:11 AM
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
Trashcart_States nextState = Trashcart_States::State_StoppedAtBase;

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

Trashcart_States WhatIsNextState(Trashcart_States currentCardState)
{
	Trashcart_States nextState = currentCardState;
	if (currentCardState == Trashcart_States::State_StoppedAtBase || currentCardState == Trashcart_States::State_ReachedPickupPoint)
	{
		// timer triggered
		if (currentCardState == Trashcart_States::State_StoppedAtBase)
		{
			if (TimeForTrashToBePicked() == true)
				nextState = Trashcart_States::State_DrivingToPickupPoint;
		}
		else if (TimeForReturning())
		{
			nextState = Trashcart_States::State_ReturningToBase;
		}
	}

	return nextState;
}

MovementState GetDrivingEvent()
{
	// collect all sensors data (time, color, ultrasonic)
	ReadColorSensor(controlData.color);
	controlData.obstructionDistance = DetectObstructionDistance();
	return DetermineTrashCanMovement(controlData);
}

void loop()
{
	Trashcart_States nextState = WhatIsNextState(currentState);
	currentState = nextState;
	MovementState drivingEvent = MovementState::Continue;

	switch (nextState)
	{
	case Trashcart_States::State_StoppedAtBase:
		currentState = Trashcart_States::State_StoppedAtBase;
		break;

	case Trashcart_States::State_DrivingToPickupPoint:
		currentState = Trashcart_States::State_DrivingToPickupPoint;
		trashcartMovement = GetDrivingEvent();
		break;

	case Trashcart_States::State_ReachedPickupPoint:
		currentState = Trashcart_States::State_ReachedPickupPoint;
		break;

	case Trashcart_States::State_ReturningToBase:
		currentState = Trashcart_States::State_ReturningToBase;
		trashcartMovement = GetDrivingEvent();
		break;
	}

	switch (trashcartMovement)
	{
	case MovementState::Continue:
		// continue what ever was happening
		break;	

	case MovementState::GoForward:
		MoveForward();
		break;

	case MovementState::TurnRight:
		RightTurn();
		break;

	case MovementState::TurnLeft:
		LeftTurn();
		break;

	case MovementState::BackupAndTurnRight:
	{
		Backup();

		// Now turn right
		RightTurn();
		delay(1000);
	}
	break;

	case MovementState::BackupAndTurnLeft:
	{
		Backup();

		// Now turn left
		LeftTurn();
		delay(1000);
	}
	break;

	case MovementState::GoBack:
		Reverse();
		break;

	case MovementState::Obstruction:
		AvoidObstruction();
		break;

	case MovementState::Stop:
		StopMovement();

		if (ReturningToBase == true)
			ReverseInPosition();

		// wait for trigger to restart the drive
		{
			unsigned long WaitForSeconds = DetermineWaitTimeBeforeReturningBackToBase();
			delay(WaitForSeconds);
		}

		TurnAround();
		break;
	}
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

unsigned long DetermineWaitTimeBeforeReturningBackToBase()
{
	unsigned long waitTime = 0;

#ifdef SIMULATE_WAIT_TIME
	waitTime = SIMULATION_WAIT_TIME * MILLISECONDS;
#else
	if (ReturningToBase == true)
	{
		// wait for next pickup time which is 7 days aways - 6 hours
	}
	else
	{
		waitTime = 6 * 60 * 60 * MILLISECONDS;	// 6 hours
	}
#endif

	return waitTime;
}

void TurnAround()
{
	{
		ReturningToBase = true;

		// SIMULATING RETURN BACK TO BASE POSITION
		Reverse();
		delay(1000);

		analogWrite(leftMotorEnablePin, 255);
		analogWrite(rightMotorEnablePin, 255);
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		delay(6000);

		MoveForward();
		delay(1000);
	}
}

void ReverseInPosition()
{
}

bool TimeForTrashToBePicked()
{
	delay(30000);
	return true;
}

bool TimeForReturning()
{
	delay(15000);
	return true;
}
