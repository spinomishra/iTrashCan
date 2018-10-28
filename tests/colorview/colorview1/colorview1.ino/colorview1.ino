#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
/*#define redpin 3
#define greenpin 5
#define bluepin 6*/
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

void setup() {
  Serial.begin(9600);
  Serial.println("Color View Test!");

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
 /* // use these three pins to drive an LED
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  */
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;      
    }
    //Serial.println(gammatable[i]);
  }
}

void ConvertColor2RGB(uint16_t red, uint16_t green, uint16_t blue, uint16_t clear)
{
  uint32_t color = red | green | blue | clear ;
  Serial.print(" Color value: "); Serial.print(color) ; Serial.print(" Hex: ") ; Serial.print((int)color, HEX);

  uint16_t R = (uint8_t) ((red >> 16) & 0xFF) ;
  uint16_t G = (uint8_t) (green >> 8) >> 0xFF  ;
  uint16_t B = (uint16_t) (blue & 0xFF) ;
    
  Serial.print(" RGB (0-255) R: ");
  Serial.print((int)R); 
  Serial.print(" G: ");Serial.print(G); 
  Serial.print(" B: ");Serial.print(B);
  Serial.println();
}

void LogColorData(uint16_t red, uint16_t green, uint16_t blue, uint16_t clear)
{
  Serial.print(" R: ");
  Serial.print((int)red, HEX); 
  Serial.print(" G: ");Serial.print((int)green, HEX); 
  Serial.print(" B: ");Serial.print((int)blue, HEX);
  Serial.print(" Clear: ");Serial.print((int)clear, HEX);
  Serial.println();

  ConvertColor2RGB(red,green, blue, clear) ;
}

void loop() {
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);      // turn on LED

  delay(100);  // takes 100ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);

  Serial.print("Raw Data ") ;
  LogColorData(red, green, blue, clear);
  
  //Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );

  //analogWrite(redpin, gammatable[(int)r]);
  //analogWrite(greenpin, gammatable[(int)g]);
  //analogWrite(bluepin, gammatable[(int)b]);


  uint16_t ir = (red + green + blue > clear) ? (red + green + blue - clear) / 2 : 0;
  uint16_t r_comp = red - ir;
  uint16_t g_comp = green - ir;
  uint16_t b_comp = blue - ir;
  uint16_t c_comp = clear - ir;   

  Serial.print("Compenseted Data ") ;
  LogColorData(r_comp, g_comp, b_comp, c_comp);  
}
