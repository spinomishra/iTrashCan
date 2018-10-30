#pragma once

// Left Motor
int leftMotorEnablePin = 9;
int leftMotorInput1 = 8;
int leftMotorInput2 = 7;

// Right Motor
int rightMotorEnablePin = 3;
int rightMotorInput1 = 5;
int rightMotorInput2 = 4;

// Ultrasonic sensor
#define TRIGGER_PIN  10 /* sends sound wave */
#define ECHO_PIN     13 /* reads bounced sound wave */
#define MAX_DISTANCE 400 /* max distance in cm for sound wave to travel */

