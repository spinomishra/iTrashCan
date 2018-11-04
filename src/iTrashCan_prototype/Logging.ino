#include <Wire.h>
#include "ColorDefinitions.h"


void LogColor(const String& header, Color color)
{
	Serial.print(header);
	Serial.print(color.R);
	Serial.print(","); Serial.print(color.G);
	Serial.print(","); Serial.print(color.B);
	Serial.print(","); Serial.println(color.WhiteLight);
}

