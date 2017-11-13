#pragma once
#include <windows.h>

#include "mdc_t.h"
#include "temp_color_setter_t.h"
#include "window_rect_t.h"


struct mdc2_t : public mdc_t
{
	mdc2_t(HDC hdc);
	
	void clear(UINT id);
	void clear(color_t c);
};
