#pragma once
#include <windows.h>

#include "errors.h"
#include "temp_object_t.h"
#include "window_rect_t.h"


struct clipper_t
{
	HDC hdc;
	
	clipper_t(HDC hdc);

	void clear();
	void fill();
	void merge(int x, int y, int w, int h);
	void cut(int x, int y, int w, int h);
	void crop(int x, int y, int w, int h);
};
