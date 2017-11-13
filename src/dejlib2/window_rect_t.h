#pragma once
#include <windows.h>

#include "rect_t.h"


struct window_rect_t : public rect_t
{
	HWND parent;
	
	window_rect_t(HWND parent, bool client=false);
	window_rect_t(HDC hdc, bool client=false);
	
	void update(bool client=false);
	SIZE size() const;
	POINT pos() const;
};
