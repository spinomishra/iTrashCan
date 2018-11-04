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
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);

Servo ussServo;	// servo that holds ultra sonic sensor

float duration, distance;

// path direction is initially set to stop
DriveState CurrentTrashCanMovement = DriveState::Stop ; 
ControlData controlData;

// Determining the path direction for the trash bin to or from curb side
DriveState DetermineTrashCanMovement(const ControlData& controlData)
{
	// go back if there is an obstruction within 15 cm
	if (controlData.obstructionDistance != 0 && controlData.obstructionDistance <= SAFEDISTANCE)
		return DriveState::GoBack;

	// we are sensing RED color then stop
	CurrentTrashCanMovement = DriveState::GoForward;

	int R = controlData.color.R;
	int G = controlData.color.G;
	int B = controlData.color.B;

	int deltaRA = ((R - G) * 100) / R;
	int deltaRB = ((R - B) * 100) / R;
	int deltaGA = ((G - R) * 100) / G;
	int deltaGB = ((G - B) * 100) / G;
	int deltaBA = ((B - R) * 100) / B;
	int deltaBB = ((B - G) * 100) / B;

	return DriveState::GoForward;

	Serial.print("Red Dominance :"); Serial.print(deltaRA); Serial.print(" "); Serial.println(deltaRB);
	if (deltaRA >= 50 || deltaRB >= 50)  // Red dominance
	{
		return DriveState::Stop;
	}

	Serial.print("Green Dominance :"); Serial.print(deltaGA); Serial.print(" "); Serial.println(deltaGB);
	if (deltaGA >= 35 || deltaGB >= 35)  // Green dominance
	{
		return DriveState::GoForward;
	}

	Serial.print("Blue Dominance :"); Serial.print(deltaBA); Serial.print(" "); Serial.println(deltaBB);
	if (deltaBA >= 35 || deltaBB >= 35)  // Blue dominance
	{
		return DriveState::TurnLeft;
	}

	return CurrentTrashCanMovement;
}

void ConvertColor2RGB(Color& color)
{
	uint32_t colorValue = color.R | color.G | color.B ;

	Serial.print(" Color value: ");  Serial.print(colorValue);  
	Serial.print(" Hex: ");   Serial.print(colorValue, HEX);


	Color mappedColor;
	mappedColor.R = (colorValue & 0xff0000) >> 16;
	mappedColor.G = (colorValue & 0x00ff00) >> 8;
	mappedColor.B = (colorValue & 0x0000ff);
	mappedColor.WhiteLight = color.WhiteLight;

	LogColor(" RGB (0-255) :", mappedColor);

	color.R = mappedColor.R;
	color.G = mappedColor.G;
	color.B = mappedColor.B;
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

			analogWrite(rightMotorEnablePin,turnspeed);
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

			for (int i = 80, j=100; i >= 20, j<=140; i -= 20, j+=20)
			{
				delay(200);
				ussServo.write(i);
				duration = sonar.ping();
				distance = (duration / 2) * 0.0343;

				if (distance >= PASSTHRUDISTANCE)
				{
					// next movement direction = right and angle of turn = 90-i
					turnangle = 90 - i;
					turnDirection = DriveState::TurnRight;

					break;
				}

				delay(200);
				ussServo.write(j);
				duration = sonar.ping();
				distance = (duration / 2) * 0.0343;
				if (distance >= PASSTHRUDISTANCE)
				{
					// next movement direction = left and angle of turn = j-90
					turnangle = j - 90;
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

	analogWrite(leftMotorEnablePin, 175);
	analogWrite(rightMotorEnablePin, 175);

	ussServo.attach(ULTRASONIC_SERVO_PIN);
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

	Serial.print("Distance @ loop = "); Serial.println(distance);
	if (controlData.obstructionDistance >= OUTOFRANGE) {
		Serial.println("Out of range");
	}
	// ********** for prototype we are not using time sensor ***********

	DriveState trashCanMovement = DetermineTrashCanMovement(controlData);
	switch (trashCanMovement)
	{
	case DriveState::GoForward: {
		Serial.println(" Setting motor to FORWARD ******");
		Serial.println("  ");
		Serial.println("  ");
		analogWrite(leftMotorEnablePin, 105);
		analogWrite(rightMotorEnablePin, 105);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
	}
								break;

	case DriveState::TurnRight: {
		Serial.println(" Setting motor to RIGHT |||***");
		Serial.println("  ");
		Serial.println("  ");
		analogWrite(leftMotorEnablePin, 225);
		analogWrite(rightMotorEnablePin, 105);
		digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, HIGH);
		delay(1000);

		analogWrite(leftMotorEnablePin, 105);
		analogWrite(rightMotorEnablePin, 105);
			digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
	}
								break;

	case DriveState::TurnLeft: {
		Serial.println(" Setting motor to LEFT ***|||");
		Serial.println("  ");
		Serial.println("  ");

		analogWrite(leftMotorEnablePin, 105);
		analogWrite(rightMotorEnablePin, 225);
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, HIGH);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
		delay(1000);

		analogWrite(leftMotorEnablePin, 105);
		analogWrite(rightMotorEnablePin, 105);
			digitalWrite(leftMotorInput1, HIGH);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, HIGH);
		digitalWrite(rightMotorInput2, LOW);
	}
	break;

	case DriveState::GoBack:
	{
		Serial.println(" Setting motor to REVERSE ||||||");
		AvoidObstruction();
	}
	break;

	default: {
		Serial.println(" Setting motor to STOP ------");
		Serial.println("  ");
		Serial.println("  ");
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, LOW);

		/* digitalWrite(leftMotorInput1, LOW);
		 digitalWrite(leftMotorInput2, LOW);
		 digitalWrite(rightMotorInput1, LOW);
		 digitalWrite(rightMotorInput2, LOW);
	   delay(700);*/
	}
			 break;
	}

	//Really close distance.
	/*
	if(distance <= 25){

		Serial.println("Setting motor to BACKWARDS ||||||");
		Serial.println("Distance is less than 25 cm");
		Serial.println("  ") ;
		Serial.println("  ") ;
		digitalWrite(leftMotorInput1, LOW);
		digitalWrite(leftMotorInput2, LOW);
		digitalWrite(rightMotorInput1, LOW);
		digitalWrite(rightMotorInput2, LOW);
	  delay(750);
	  }

	*/
}
