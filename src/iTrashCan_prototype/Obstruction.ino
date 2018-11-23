
//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 11/4/2018T7:36 PM
//

#include <NewPing.h>
#include <Servo.h>

#define USE_SERVO

// Ultra sonic definition

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Servo ussServo;	// servo that holds ultra sonic sensor

void EnableObstructionDetection()
{
	ussServo.attach(ULTRASONIC_SERVO_PIN);
	ussServo.write(USS_SERVO_BASE_ANGLE);
}

void PointUltrasonicSensorAtAngle(int angle)
{
	ussServo.write(angle);
}

void DisableObstructionDetection()
{
	ussServo.detach();
}

float DetectObstructionDistance()
{
	EnableObstructionDetection();

	float distance = SonarDistance();

	DisableObstructionDetection();
	return distance;
}

float SonarDistance()
{
	// first read ultrasonic sensor raw value
	unsigned int duration = sonar.ping();
	
	// Determine distance from duration
	// using 343 meters per second as speed of sound
	float distance = (duration / 2) * 0.0343;
	Serial.print("Ultrasonic sensor - object distance : "); Serial.println(distance);

	return distance;
}

MovementState FindClearPath(int& turnangle)
{
	float distance = 0;
	
	MovementState turnDirection = MovementState::Stop;
	EnableObstructionDetection();

	// move from 60 degree angle to 140 degree looking if there is an obstruction
	// similarly move from 100 degree to 20 degree looking if there is any obstruction
	// which ever angle we find there is no obstruction is our clear path
	for (int i = 60, j = 100; i >= 20, j <= 140; i -= 20, j += 20)
	{
		PointUltrasonicSensorAtAngle(i);
		distance = SonarDistance();

		if (distance >= PASSTHRUDISTANCE)
		{
			// next movement direction = right and angle of turn = BaseAngle-i
			turnangle = USS_SERVO_BASE_ANGLE - i;
			turnDirection = MovementState::TurnRight;

			break;
		}
		
		PointUltrasonicSensorAtAngle(j);
		distance = SonarDistance();

		if (distance >= PASSTHRUDISTANCE)
		{
			// next movement direction = left and angle of turn = BaseAngle-90
			turnangle = j - USS_SERVO_BASE_ANGLE;
			turnDirection = MovementState::TurnLeft;
			break;
		}
	}

	return turnDirection;
}