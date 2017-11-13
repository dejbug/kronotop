#pragma once
#include <windows.h>

#include "errors.h"
#include "error_t.h"
#include "window_rect_t.h"


struct window_positioner_t
{
	HWND handle;
	UINT extra_flags;
	
	window_positioner_t(HWND handle);
	
	void move(int dx, int dy) const;
	void setpos(int x, int y) const;
	void setsize(int w, int h) const;
	void below(HWND other) const;
	void to_bottom() const;
	void to_top() const;
	void to_permanent_top() const;
	void to_quasi_top() const;
	void center_to_screen() const;
};
