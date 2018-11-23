//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 11/4/2018T2:36 PM
//
#include <Adafruit_TCS34725.h>

#define MAX_COLOR_COMPONENT_VALUE 21504
#define MAX_RGB_VALUE 255

// Color sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);

void DetectColorSensor()
{
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
}

void ReadColorSensor(Color& color)
{
	// Read the raw red, green, blue and clear channel values. Will return
	// red, green, blue and clear color values.
	tcs.getRawData(&color.R, &color.G, &color.B, &color.WhiteLight);
	tcs.setInterrupt(true);

	LogColor("Raw RGBC :", color);

	// raw values are big individual values. We will need to convert them to RGB range 0 to 255.
	ConvertColor2RGB(color);
}

void ConvertColor2RGB(Color& color)
{
	uint32_t colorValue = color.R | color.G | color.B;

	Serial.print("Color value: ");  Serial.print(colorValue);
	Serial.print("Hex: ");   Serial.print(colorValue, HEX);

	Color mappedColor;
	mappedColor.R = map(color.R, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);
	mappedColor.G = map(color.G, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);
	mappedColor.B = map(color.B, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);

	// note: white light plays a big light in the color value for RGB. The more light 
	// we have the better color value. The less light we have lower the values.
	mappedColor.WhiteLight = map(color.WhiteLight, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);

	LogColor("RGB (0-255) :", mappedColor);

	color.R = mappedColor.R;
	color.G = mappedColor.G;
	color.B = mappedColor.B;
	color.WhiteLight = mappedColor.WhiteLight;
}

void LogColor(const String& header, Color color)
{
	Serial.print(header);
	Serial.print(color.R);
	Serial.print(","); Serial.print(color.G);
	Serial.print(","); Serial.print(color.B);
	Serial.print(","); Serial.println(color.WhiteLight);
}