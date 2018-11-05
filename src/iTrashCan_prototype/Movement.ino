DriveState CurrentTrashCanMovement = DriveState::Stop;

// Determining the path direction for the trash bin to or from curb side
DriveState DetermineTrashCanMovement(const ControlData& controlData)
{
	// go back if there is an obstruction within 15 cm
	if (controlData.obstructionDistance != 0 && controlData.obstructionDistance <= SAFEDISTANCE)
		return DriveState::GoBack;

	CurrentTrashCanMovement = DriveState::GoForward;

	int R = controlData.color.R;
	int G = controlData.color.G;
	int B = controlData.color.B;

	if (R >= 135 && R > B && R > G)  // Red dominance
	{
		Serial.println("Red Dominance :");
		CurrentTrashCanMovement = DriveState::Stop;
	}

	if (G >= 60 && G >= B)  // Green dominance
	{
		Serial.println("Green Dominance :");
		CurrentTrashCanMovement = DriveState::TurnLeft;
	}

	if (B >= 90 || B > R && B >= G)  // Blue dominance
	{
		Serial.println("Blue Dominance :");
		CurrentTrashCanMovement = DriveState::TurnRight;
	}

	return CurrentTrashCanMovement;
}

void TurnToAvoidObstruction(int turnangle, DriveState turnDirection)
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
	case DriveState::TurnLeft:
		Serial.println("Turning left to avoid obstruction");
		analogWrite(leftMotorEnablePin, 100);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);

		analogWrite(rightMotorEnablePin, turnspeed);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		break;

	case DriveState::TurnRight:
		Serial.println("Turning right to avoid obstruction");
		analogWrite(leftMotorEnablePin, turnspeed);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);

		analogWrite(rightMotorEnablePin, 100);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		break;
	}
}

void AvoidObstruction()
{
	/*
	There is some obstruction infront. Move the trash can BACK few steps and then identify which direction is
	open for movement. Move in the open direction for few steps before straightening and going forward.
	*/
	int nBackStepsTaken = 0;
	int LeftReverse = 0;
	int RightReverse = 0;
	DriveState turnDirection = DriveState::Stop;
	int turnangle = 0;

	while (1)
	{
		delay(200);

		// first read ultrasonic sensor raw value
		unsigned int duration = sonar.ping();

		// Determine distance from duration
		// using 343 metres per second as speed of sound
		float distance = (duration / 2) * 0.0343;

		Serial.print("Obstruction distance = "); Serial.println(distance);
		if (distance != 0 && distance <= SAFEDISTANCE)
		{
			for (int i = 0; i < 5; i++) {
				// going reverse
				digitalWrite(leftMotorInput1, LOW);
				digitalWrite(leftMotorInput2, HIGH);

				digitalWrite(rightMotorInput1, LOW);
				digitalWrite(rightMotorInput2, HIGH);
			}

			for (int i = 60, j = 100; i >= 20, j <= 140; i -= 20, j += 20)
			{
				delay(200);
				ussServo.write(i);
				duration = sonar.ping();
				distance = (duration / 2) * 0.0343;

				if (distance >= PASSTHRUDISTANCE)
				{
					// next movement direction = right and angle of turn = 90-i
					turnangle = USS_SERVO_BASE_ANGLE - i;
					turnDirection = DriveState::TurnRight;

					break;
				}

				delay(250);
				ussServo.write(j);
				duration = sonar.ping();
				distance = (duration / 2) * 0.0343;
				if (distance >= PASSTHRUDISTANCE)
				{
					// next movement direction = left and angle of turn = j-90
					turnangle = j - USS_SERVO_BASE_ANGLE;
					turnDirection = DriveState::TurnLeft;
					break;
				}
			}

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

void MoveTurnRight()
{
	// first stop
	StopMovement();

	Serial.println(" Setting motor to REVERSE a bit |||***");
	while (1)
	{
		analogWrite(leftMotorEnablePin, 200);
		analogWrite(rightMotorEnablePin, 200);

		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, HIGH);
	}

	Serial.println(" Setting motor to RIGHT |||***");
	Serial.println("  ");
	Serial.println("  ");

	for (int i = 0; i < 10; i++)
	{
		analogWrite(leftMotorEnablePin, NORMALSPEED_RIGHT);
		analogWrite(rightMotorEnablePin, SLOW_WHEEL_POWER);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, HIGH);
	}
}

void MoveTurnLeft()
{
	StopMovement();

	Serial.println(" Setting motor to REVERSE a bit ***|||");
	for (int i = 0; i < 10; i++)
	{
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, HIGH);
	}
	for (int i = 0; i < 10; i++)
	{
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, HIGH);
	}

	Serial.println(" Setting motor to LEFT ***|||");
	Serial.println("  ");
	Serial.println("  ");

	for (int i = 0; i < 10; i++)
	{
		analogWrite(leftMotorEnablePin, SLOW_WHEEL_POWER);
		analogWrite(rightMotorEnablePin, NORMALSPEED_RIGHT);
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
	}
}

void Reverse()
{
	Serial.println(" Setting motor to REVERSE ||||||");
	AvoidObstruction();
}

void StopMovement()
{
	Serial.println(" Setting motor to STOP ------");
	digitalWrite(leftMotorInput1, LOW);
	digitalWrite(leftMotorInput2, LOW);
	digitalWrite(rightMotorInput1, LOW);
	digitalWrite(rightMotorInput2, LOW);
}
