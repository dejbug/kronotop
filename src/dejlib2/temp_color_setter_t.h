#pragma once
#include <windows.h>

#include "color_t.h"


struct temp_color_setter_t
{
	HDC hdc;
	HPEN const null_pen;
	HGDIOBJ old_brush, old_pen;
	
	temp_color_setter_t(HDC hdc);
	virtual ~temp_color_setter_t();
	
	void reset();
	void fb(bool fg=true, bool bg=true);
	void ff(bool fg, bool bg=false);
	void bb(bool bg, bool fg=false);
	void fg(char r, char g, char b);
	void bg(char r, char g, char b);
	void fg(COLORREF c);
	void bg(COLORREF c);
};
