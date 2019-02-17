//
// ©Copyright 2018, All Rights Reserved.
//
// PinDefinitions.h created by spinomishra on 10/‎29/‎2018T‏‎6:00 PM
//
#pragma once

// Left Motor
int leftMotorEnablePin = 11;
int leftMotorInput1 = 7;
int leftMotorInput2 = 8;

// Right Motor
int rightMotorEnablePin = 3;
int rightMotorInput1 = 5;
int rightMotorInput2 = 4;

// Ultrasonic sensor
#define TRIGGER_PIN				13  /* sends sound wave */
#define ECHO_PIN				12  /* reads bounced sound wave */
#define MAX_DISTANCE			400 /* max distance in cm for sound wave to travel */
#define ULTRASONIC_SERVO_PIN	10
