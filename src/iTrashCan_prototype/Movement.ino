//
// ©Copyright 2018, All Rights Reserved.
//
// Movement.ino created by spinomishra on 11/4/2018T12:28 PM
//

Markers DetermineMarker(Color color)
{
	int R = color.R;
	int G = color.G;
	int B = color.B;

	// amount of white light plays a big light in the color value for RGB. More white light 
	// we have, better the color value we get. The less light we have lower the values.1
	int threshold_R = (color.WhiteLight < 100) ? 50 : 100;
	int threshold_G = (color.WhiteLight < 100) ? 25 : 60;
	int threshold_B = (color.WhiteLight < 100) ? 30: 80;

	if (R >= threshold_R && R > B && R > G)  // Red dominance
	{
		Serial.println("Red Dominance :");
		return Markers::RedMarker;
	}

	if (G >= threshold_G && G >= B && G > R)  // Green dominance
	{
		Serial.println("Green Dominance :");
		return Markers::GreenMarker;
	}

	if (B >= threshold_B && B > R && B >= G)  // Blue dominance
	{
		Serial.println("Blue Dominance :");
		return Markers::BlueMarker;
	}

	return Markers::Clear;
}

void TurnToAvoidObstruction(int turnangle, MoveDirection turnDirection)
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
	case MoveDirection::TurnLeft:
		Serial.println("Turning left to avoid obstruction");
		LeftTurn(turnangle);

		//analogWrite(leftMotorEnablePin, 150);
		//digitalWrite(leftMotorInput1, HIGH);
		//digitalWrite(leftMotorInput2, LOW);

		//analogWrite(rightMotorEnablePin, turnspeed);
		//digitalWrite(rightMotorInput1, HIGH);
		//digitalWrite(rightMotorInput2, LOW);
		break;

	case MoveDirection::TurnRight:
		Serial.println("Turning right to avoid obstruction");
		RightTurn(turnangle);

		//analogWrite(leftMotorEnablePin, turnspeed);
		//digitalWrite(leftMotorInput1, HIGH);
		//digitalWrite(leftMotorInput2, LOW);

		//analogWrite(rightMotorEnablePin, 150);
		//digitalWrite(rightMotorInput1, HIGH);
		//digitalWrite(rightMotorInput2, LOW);
		break;
	}

	//delay(500);
}

void AvoidObstruction()
{
	/*
	There is some obstruction in front. Move the trash can BACK few steps and then identify which direction is
	open for movement. Move in the open direction for few steps before straightening and going forward.
	*/
	int nBackStepsTaken = 0;
	
	MoveDirection turnDirection = MoveDirection::None;
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
	int leftPwm = NORMALSPEED_LEFT;
	int rightPwm = NORMALSPEED_RIGHT;
	Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.println(rightPwm);

	analogWrite(rightMotorEnablePin, leftPwm);
	analogWrite(leftMotorEnablePin, rightPwm);

	digitalWrite(leftMotorInput1, HIGH);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, HIGH);
	digitalWrite(rightMotorInput2, LOW);
}

void RightTurn(int angle)
{
	Serial.println(" Setting motors for RIGHT TURN |||***");
	int leftPwm = 100;
	int rightPwm = 120;
	int motorRuntime = angle * 12;

	Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.print(rightPwm);
	Serial.print(" Motor Runtime = "); Serial.println(motorRuntime);

	analogWrite(leftMotorEnablePin, leftPwm);
	analogWrite(rightMotorEnablePin, rightPwm);	// lowering the speed of right motor by providing less pwms
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, HIGH);
	digitalWrite(rightMotorInput1, HIGH);
	digitalWrite(rightMotorInput2, LOW);
	delay(motorRuntime);
	StopMovement(50); 
}

void LeftTurn(int angle)
{
	Serial.println(" Setting motors for LEFT TURN |||***");
	int leftPwm = 100;
	int rightPwm = 120;

	int motorRuntime = angle * 12;
	
	Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.print(rightPwm);
	Serial.print(" Motor Runtime = "); Serial.println(motorRuntime);

	analogWrite(leftMotorEnablePin, leftPwm); // lowering the speed of left motor by providing less pwms
	analogWrite(rightMotorEnablePin, rightPwm);	
	digitalWrite(leftMotorInput1, HIGH);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, HIGH);
	delay(motorRuntime);
	StopMovement(50);
}

void StopMovement(int stopTime)
{
	Serial.println(" Setting motor to STOP ------");
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, LOW);

	if (stopTime > 0)
		delay(stopTime);
}

void Reverse()
{
	Serial.print(" Setting motors to REVERSE ||||||");

	int leftPwm = REVERSESPEED_LEFT;
	int rightPwm = REVERSESPEED_RIGHT;
	Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.println(rightPwm);

	analogWrite(leftMotorEnablePin, leftPwm);
	analogWrite(rightMotorEnablePin, rightPwm);

	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, HIGH);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, HIGH);
}

void PivotRight()
{
	Serial.println(" Setting motors for RIGHT PIVOT |||***");
	Serial.println("  ");
	Serial.println("  ");

	analogWrite(leftMotorEnablePin, 180);
	analogWrite(rightMotorEnablePin, 0);	// lowering the speed of right motor by providing less pwms
	digitalWrite(leftMotorInput1, HIGH);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, LOW);
	delay(250);
}

void PivotLeft()
{
	Serial.println(" Setting motors for LEFT PIVOT ***|||");
	Serial.println("  ");
	Serial.println("  ");

	analogWrite(leftMotorEnablePin, 0);
	analogWrite(rightMotorEnablePin, 180);	
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, HIGH);
	digitalWrite(rightMotorInput2, LOW);
	delay(500);
}

void PivotCenter()
{
	Serial.println(" Setting motors for CENTER PIVOT ***|||");

	//int leftPwm = 100;
	//int rightPwm = 100;
	//Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.println(rightPwm);

	//analogWrite(leftMotorEnablePin, leftPwm);
	//analogWrite(rightMotorEnablePin, rightPwm);

	//digitalWrite(leftMotorInput1, LOW);
	//digitalWrite(leftMotorInput2, HIGH);
	//digitalWrite(rightMotorInput1, HIGH);
	//digitalWrite(rightMotorInput2, LOW);

	//int motorRuntime = 360*11;
	//Serial.print("Turn Angle = 360, Motor Runtime = "); Serial.println(motorRuntime);
	//delay(motorRuntime);

	//Serial.println(" Making slight right TURN |||***");
	//int leftPwm = 120;
	//int rightPwm = 80;
	//int motorRuntime = 1000;

	//Serial.print(" PWMs: "); Serial.print(leftPwm); Serial.print(" "); Serial.print(rightPwm);
	//Serial.print(" Motor Runtime = "); Serial.println(motorRuntime);

	//analogWrite(leftMotorEnablePin, leftPwm);
	//analogWrite(rightMotorEnablePin, rightPwm);	// lowering the speed of right motor by providing less pwms
	//digitalWrite(leftMotorInput1, LOW);
	//digitalWrite(leftMotorInput2, HIGH);
	//digitalWrite(rightMotorInput1, HIGH);
	//digitalWrite(rightMotorInput2, LOW);
	//delay(motorRuntime);

	//Reverse();
	//delay(500);

	//analogWrite(leftMotorEnablePin, leftPwm);
	//analogWrite(rightMotorEnablePin, rightPwm);	// lowering the speed of right motor by providing less pwms
	//digitalWrite(leftMotorInput1, LOW);
	//digitalWrite(leftMotorInput2, HIGH);
	//digitalWrite(rightMotorInput1, HIGH);
	//digitalWrite(rightMotorInput2, LOW);
	//delay(motorRuntime);

	RightTurn(180);
	MoveForward();
	delay(1000);
	RightTurn(40);
	Reverse();
	delay(1000);
	StopMovement(50);

	StopMovement(20);
}

void ReverseInPosition()
{
	Color color;
	ReadColorSensor(color);
	Markers marker = DetermineMarker(color);

	while (marker != Clear)
	{
		Serial.print("marker = "); Serial.println(marker);
		Reverse();
		delay(500);

		Color color;
		ReadColorSensor(color);
		Markers marker = DetermineMarker(color);
		Serial.print("marker = "); Serial.println(marker);
		if (marker == GreenMarker)
			LeftTurn(360);
		else if (marker == BlueMarker)
			RightTurn(360);
		else if (marker == RedMarker)
			Reverse();
	}
	
	PivotCenter();
	StopMovement(250);
}

void BringItBackWithinBoundary(DriveEvent boundary)
{
/*	StopMovement(50);

	// Check if we are still sensing left boundary or we have crossed it
	int runTime = 5;
	bool turn = true;
	while (1)
	{
		Color color;
		ReadColorSensor(color);
		Markers marker = DetermineMarker(color);
		switch (marker)
		{
		case GreenMarker:
			
		case BlueMarker:
			if (boundary == DriveEvent_CrossedLeftBoundary)
				RightTurn(80);
			else
				LeftTurn(80);

			runTime = 1000;
			break;

		case Clear:
			runTime = 500;
			break;
		}

		Serial.print("Moving Forward for "); 
		Serial.print(runTime);

		MoveForward();
		delay(runTime);

		Serial.print(" Stopping for 50 ms");
		StopMovement(50);

		if (runTime == 1000)
			break;
	}*/
}