//
// ©Copyright 2018, All Rights Reserved.
//
// ColorDefinitions.h created by spinomishra on 10/29/2018T10:49 PM
//
#pragma once

struct Color {
	uint16_t R;
	uint16_t G;
	uint16_t B;
	uint16_t WhiteLight;
	uint16_t Lux;
	uint16_t Temp;
};

enum ColorMarkers {
	RED,
	BLUE,
	YELLOW,
	GREEN,
	WHITE
};
