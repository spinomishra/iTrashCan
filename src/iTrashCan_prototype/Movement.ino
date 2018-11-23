//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 11/4/2018T12:28 PM
//

MovementState CurrentTrashCanMovement = MovementState::Stop;

MovementState DetermineMoveDirection(Color color)
{
	int R = color.R;
	int G = color.G;
	int B = color.B;

	// amount of white light plays a big light in the color value for RGB. More white light 
	// we have, better the color value we get. The less light we have lower the values.
	int threshold_R = (color.WhiteLight < 100) ? 50 : 135;
	int threshold_G = (color.WhiteLight < 100) ? 25 : 60;
	int threshold_B = (color.WhiteLight < 100) ? 30: 90;

	if (R >= threshold_R && R > B && R > G)  // Red dominance
	{
		Serial.println("Red Dominance :");
		return MovementState::Stop;
	}

	if (G >= threshold_G && G >= B)  // Green dominance
	{
		Serial.println("Green Dominance :");
		return MovementState::BackupAndTurnLeft;
	}

	if (B >= threshold_B && B > R && B >= G)  // Blue dominance
	{
		Serial.println("Blue Dominance :");
		return MovementState::BackupAndTurnRight;
	}

	return MovementState::GoForward;
}

// Determining the path direction for the trash bin to or from curb side
MovementState DetermineTrashCanMovement(const ControlData& controlData)
{
	// go back if there is an obstruction within 15 cm
	if (controlData.obstructionDistance != 0 && controlData.obstructionDistance <= SAFEDISTANCE)
		return MovementState::Obstruction;

	CurrentTrashCanMovement = DetermineMoveDirection(controlData.color);

	return CurrentTrashCanMovement;
}

void TurnToAvoidObstruction(int turnangle, MovementState turnDirection)
{
	int turnspeed = 255;

	////////////////////////
	if (turnangle >= 20 && turnangle <40) {
		turnspeed = 205;
	}

	if (turnangle >= 40 && turnangle <60) {
		turnspeed = 175;
	}

	if (turnangle >= 60 && turnangle <80) {
		turnspeed = 135;
	}
	/////////////////////////////////

	switch (turnDirection)
	{
	case MovementState::TurnLeft:
		Serial.println("Turning left to avoid obstruction");
		analogWrite(leftMotorEnablePin, 150);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);

		analogWrite(rightMotorEnablePin, turnspeed);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		break;

	case MovementState::TurnRight:
		Serial.println("Turning right to avoid obstruction");
		analogWrite(leftMotorEnablePin, turnspeed);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);

		analogWrite(rightMotorEnablePin, 150);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		break;
	}

	delay(500);
}

void AvoidObstruction()
{
	/*
	There is some obstruction in front. Move the trash can BACK few steps and then identify which direction is
	open for movement. Move in the open direction for few steps before straightening and going forward.
	*/
	int nBackStepsTaken = 0;
	
	MovementState turnDirection = MovementState::Stop;
	int turnangle= 0;

	while (1)
	{
		float distance = DetectObstructionDistance();

		if (distance != 0 && distance <= SAFEDISTANCE)
		{
			turnDirection = FindClearPath(turnangle);

			// going reverse
			Reverse();
			delay(500); // reversing for 1/2 seconds before looking for a way	

			TurnToAvoidObstruction(turnangle, turnDirection);
		}
		else
		{
			break;
		}
	}

	Serial.println("Successfully avoided obstruction");
}

void MoveForward()
{
	Serial.println(" Setting motor to FORWARD ******");
	Serial.println("  ");
	Serial.println("  ");
	analogWrite(rightMotorEnablePin, NORMALSPEED_LEFT);
	analogWrite(leftMotorEnablePin, NORMALSPEED_RIGHT);
	digitalWrite(leftMotorInput1, HIGH);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, HIGH);
	digitalWrite(rightMotorInput2, LOW);
}

void RightTurn()
{
	Serial.println(" Setting motors for RIGHT TURN |||***");
	Serial.println("  ");
	Serial.println("  ");

	analogWrite(leftMotorEnablePin, NORMALSPEED_RIGHT);
	analogWrite(rightMotorEnablePin, NORMALSPEED_LEFT);	// lowering the speed of right motor by providing less pwms
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, HIGH);
	digitalWrite(rightMotorInput1, HIGH);
	digitalWrite(rightMotorInput2, LOW);
}

void LeftTurn()
{
	Serial.println(" Setting motors for LEFT TURN ***|||");
	Serial.println("  ");
	Serial.println("  ");

	analogWrite(leftMotorEnablePin, 150); // lowering the speed of left motor by providing less pwms
	analogWrite(rightMotorEnablePin, 255);
	digitalWrite(leftMotorInput1, HIGH);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, HIGH);
}

void Reverse()
{
	Serial.println(" Setting motors to REVERSE ||||||");
	analogWrite(leftMotorEnablePin, 255);
	analogWrite(rightMotorEnablePin, 255);

	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, HIGH);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, HIGH);
}

void StopMovement()
{
	Serial.println(" Setting motor to STOP ------");
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, LOW);
}
