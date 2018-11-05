#define MAX_COLOR_COMPONENT_VALUE 21504
#define MAX_RGB_VALUE 255

void ConvertColor2RGB(Color& color)
{
	uint32_t colorValue = color.R | color.G | color.B;

	Serial.print(" Color value: ");  Serial.print(colorValue);
	Serial.print(" Hex: ");   Serial.print(colorValue, HEX);

	Color mappedColor;
	mappedColor.R = map(color.R, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);
	mappedColor.G = map(color.G, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);
	mappedColor.B = map(color.B, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);
	mappedColor.WhiteLight = map(color.WhiteLight, 1, MAX_COLOR_COMPONENT_VALUE, 0, MAX_RGB_VALUE);

	LogColor(" RGB (0-255) :", mappedColor);

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