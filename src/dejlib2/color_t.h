#pragma once
#include <windows.h>
#include <math.h>
#include <stdio.h>


COLORREF hsv(float h, float s, float v);

struct color_t
{
	COLORREF c;
	
	color_t(COLORREF c);
	color_t(char r, char g, char b);
	color_t(int h, bool soft, bool bright);
	color_t(int h, float s, float v);
	color_t(int h, double s, double v);

	void print();
};
