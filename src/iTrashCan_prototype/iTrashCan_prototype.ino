#include <Wire.h>
#include <Servo.h>
#include <Adafruit_TCS34725.h>
#include <NewPing.h>
#include "ColorDefinitions.h"
#include "iTrashCan_Definitions.h"
#include "PinDefinitions.h"
#include <time.h>

// Ultra sonic definition
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Color sensor definition
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);

Servo ussServo;	// servo that holds ultra sonic sensor

// path direction is initially set to stop
ControlData controlData;

/* output input, true false*/
void setup()
{
	Serial.begin(9600);

	bool colorSensorNotFound = true;
	
	while (colorSensorNotFound)
	{
		if (tcs.begin()) {
			Serial.println("Found sensor");
			colorSensorNotFound = false;
		}
		else {
			Serial.println("No TCS34725 found ... check your connections");
			delay(1000); // delay 1 second and check again
		}
	}

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

	StopMovement();
}

void loop()
{
	// collect all sensors data (time, color, ultrasonic)

	// Read the raw red, green, blue and clear channel values. Will return
	// red, green, blue and clear color values.
	tcs.getRawData(&controlData.color.R, &controlData.color.G, &controlData.color.B, &controlData.color.WhiteLight);
	tcs.setInterrupt(true);

	LogColor("Raw RGBC :", controlData.color);

	// raw values are big individual values. We will need to convert them to RGB range 0 to 255.
	ConvertColor2RGB(controlData.color);

	// first read ultrasonic sensor raw value
	unsigned int duration = sonar.ping();

	// Determine distance from duration
	// using 343 metres per second as speed of sound
	controlData.obstructionDistance = (duration / 2) * 0.0343;

	Serial.print("Distance @ loop = "); Serial.println(controlData.obstructionDistance);
	if (controlData.obstructionDistance >= OUTOFRANGE)
		Serial.println("Out of range");

	// ********** for prototype we are not using time sensor ***********
	DriveState trashCanMovement = DetermineTrashCanMovement(controlData);
	switch (trashCanMovement) 
	{
		case DriveState::GoForward:
			MoveForward();
			break;

		case DriveState::TurnRight:
			MoveTurnRight();
			break;

		case DriveState::TurnLeft:
			MoveTurnLeft();
			break;

		case DriveState::GoBack:
			Reverse();
			break;

		case DriveState::Stop:
			StopMovement();
			break;
	}

}
