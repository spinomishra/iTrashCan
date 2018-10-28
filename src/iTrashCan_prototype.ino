#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "NewPing.h"

// Left Motor
int leftMotorEnablePin = 9;
int lefttMotorInput1 = 8;
int lefttMotorInput2 = 7;

// Right Motor
int rightMotorEnablePin = 3;
int rightMotorInput1 = 5;
int rightMotorInput2 = 4;

// Ultrasonic sensor
#define TRIGGER_PIN  10 /* sends sound wave */
#define ECHO_PIN     13 /* reads bounced sound wave */
#define MAX_DISTANCE 400 /* max distance in cm for sound wave to travel */

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Color sensor definition
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);

float duration, distance;
#define commonAnode true

byte gammatable[256];

enum PathDirection{
  Stop = 1,
  GoForward,
  TurnLeft,
  TurnRight,
  GoBack,
  Continue
};

struct Color {
  float R ;
  float G ;
  float B ;
  int WhiteLight;
};

enum ColorMarkers {
  RED,
  BLUE,
  YELLOW,
  GREEN,
  WHITE
} ;

// path direction is initially set to stop
PathDirection CurrentPathDirection = PathDirection::Stop ; 

// Determininig the path direction for the trash bin to or from curb side
PathDirection DeterminePathDirection(struct Color& c)
{
  int R = (int) c.R ;
  int G = (int) c.G ;
  int B = (int) c.B ;

  rgbColor.r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
  rgbColor.g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
  rgbColor.b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byt
   
  int deltaRA = ((R - G) * 100)/R ;
  int deltaRB = ((R - B) * 100)/R ;
  int deltaGA = ((G - R) * 100)/G ;
  int deltaGB = ((G - B) * 100)/G ;
  int deltaBA = ((B - R) * 100)/B ;
  int deltaBB = ((B - G) * 100)/B ;
  
  return PathDirection::GoForward ; 
  
  Serial.print("Red Dominance :");Serial.print(deltaRA) ; Serial.print(" ") ; Serial.println(deltaRB) ;
  if (deltaRA >=50 || deltaRB >= 50)  // Red dominance
  {
    return PathDirection::Stop;
  }
  
  Serial.print("Green Dominance :");Serial.print(deltaGA) ; Serial.print(" ") ; Serial.println(deltaGB) ;
  if (deltaGA >=35 || deltaGB >= 35)  // Green dominance
  {
    return PathDirection::GoForward ; 
  }
  
  Serial.print("Blue Dominance :");Serial.print(deltaBA) ; Serial.print(" ") ; Serial.println(deltaBB) ;
  if (deltaBA >=35 || deltaBB >= 35)  // Blue dominance
  {
      return PathDirection::TurnLeft ; 
  }
  
  return CurrentPathDirection ;
}

void convertTo255Range(float& red, float& green, float& blue)
{
  float normalizationVal = 255/1.75 ;

  red *= normalizationVal ;
  green *= normalizationVal ;
  blue *= normalizationVal ;
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  }
  else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
  }

  pinMode(leftMotorEnablePin, OUTPUT);
  pinMode(lefttMotorInput1, OUTPUT);
  pinMode(lefttMotorInput2, OUTPUT);

  pinMode(rightMotorEnablePin, OUTPUT);
  pinMode(rightMotorInput1, OUTPUT);
  pinMode(rightMotorInput2, OUTPUT);
  
  analogWrite(leftMotorEnablePin, 175);
  analogWrite(rightMotorEnablePin, 175);
}


void loop() {
  uint16_t clear, red, green, blue;
  
  delay(10);

  // Read the raw red, green, blue and clear channel values. Will return
  // red, green, blue and clear color values.
  tcs.getRawData(&red, &green, &blue, &clear);
  
  Serial.print(" RGBC: "); Serial.print(red);
  Serial.print(" "); Serial.print(green);
  Serial.print(" "); Serial.print(blue);
  Serial.print(" "); Serial.println(clear);

  tcs.setInterrupt(true);

  float average = (red + green + blue) / 3;
  Color color;
  
  // normalizing the color RGB values
  color.R = red / average;
  color.G = green / average;
  color.B = blue / average;
  color.WhiteLight = clear ;
  
  Serial.print(" Normalized: RGB ");
  Serial.print(color.R); Serial.print(" "); 
  Serial.print(color.G); Serial.print(" "); 
  Serial.println(color.B);

  convertTo255Range(color.R, color.G, color.B) ;
  Serial.print(" RGB (0-255)");
  Serial.print(color.R); Serial.print(" "); 
  Serial.print(color.G); Serial.print(" "); 
  Serial.println(color.B);
  
  duration = sonar.ping();

  // Determine distance from duration
  // using 343 metres per second as speed of sound
  distance = (duration / 2) * 0.0343;

  // Send results to Serial Monitor
  Serial.print("Distance = ");
  if (distance >= 400 ) {
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
    
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  PathDirection pathDirection = DeterminePathDirection(color);
  switch (pathDirection)
  { 
    case PathDirection::GoForward: {
        Serial.println(" Setting motor to FORWARD ******") ;
        Serial.println("  ") ;
        Serial.println("  ") ;
        digitalWrite(lefttMotorInput1, LOW);
        digitalWrite(lefttMotorInput2, HIGH);
        digitalWrite(rightMotorInput1, LOW);
        digitalWrite(rightMotorInput2, HIGH);
    }
    break;
    
    case PathDirection::TurnRight: {
        Serial.println(" Setting motor to RIGHT |||***") ;
        Serial.println("  ") ;
        Serial.println("  ") ;
        analogWrite(leftMotorEnablePin, 225);
        analogWrite(rightMotorEnablePin, 105);
        digitalWrite(lefttMotorInput1, LOW);
        digitalWrite(lefttMotorInput2, HIGH);
        digitalWrite(rightMotorInput1, LOW);
        digitalWrite(rightMotorInput2, HIGH);
        delay(1000);
        
        analogWrite(leftMotorEnablePin, 175);
        analogWrite(rightMotorEnablePin, 175);
        digitalWrite(lefttMotorInput1, LOW);
        digitalWrite(lefttMotorInput2, HIGH);
        digitalWrite(rightMotorInput1, LOW);
        digitalWrite(rightMotorInput2, HIGH);    
    }
    break;
    
    case PathDirection::TurnLeft: {
      Serial.println(" Setting motor to LEFT ***|||") ;
      Serial.println("  ") ;
      Serial.println("  ") ;
  
        analogWrite(leftMotorEnablePin, 105);
        analogWrite(rightMotorEnablePin, 225);
      digitalWrite(lefttMotorInput1, LOW);
      digitalWrite(lefttMotorInput2, HIGH);
      digitalWrite(rightMotorInput1, LOW);
      digitalWrite(rightMotorInput2, HIGH);
      delay(1000);
      
        analogWrite(leftMotorEnablePin, 175);
        analogWrite(rightMotorEnablePin, 175);
      digitalWrite(lefttMotorInput1, LOW);
      digitalWrite(lefttMotorInput2, HIGH);
      digitalWrite(rightMotorInput1, LOW);
      digitalWrite(rightMotorInput2, HIGH);
    }
    break ;

    case PathDirection::GoBack:
    {
        Serial.println(" Setting motor to REVERSE ||||||") ;
        Serial.println("  ") ;
        Serial.println("  ") ;
    }
    break ;
    
    default: {
        Serial.println(" Setting motor to STOP ------") ;
        Serial.println("  ") ;
        Serial.println("  ") ;
        digitalWrite(lefttMotorInput1, LOW);
        digitalWrite(lefttMotorInput2, LOW);
        digitalWrite(rightMotorInput1, LOW);
        digitalWrite(rightMotorInput2, LOW);
        
       /* digitalWrite(lefttMotorInput1, LOW);
        digitalWrite(lefttMotorInput2, LOW);
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
    digitalWrite(lefttMotorInput1, LOW);
    digitalWrite(lefttMotorInput2, LOW);
    digitalWrite(rightMotorInput1, LOW);
    digitalWrite(rightMotorInput2, LOW);
  delay(750);  
  }

*/
}
