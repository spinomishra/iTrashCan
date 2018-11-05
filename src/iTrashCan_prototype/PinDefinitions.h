#pragma once

// Left Motor
int leftMotorEnablePin =9;
int leftMotorInput1 = 8;
int leftMotorInput2 = 7;

// Right Motor
int rightMotorEnablePin = 3;
int rightMotorInput1 = 5;
int rightMotorInput2 = 4;

int turnspeed;


// Ultrasonic sensor
#define TRIGGER_PIN				13  /* sends sound wave */
#define ECHO_PIN				10  /* reads bounced sound wave */
#define MAX_DISTANCE			400 /* max distance in cm for sound wave to travel */
#define ULTRASONIC_SERVO_PIN	12
