#pragma once
#include <windows.h>

#include "errors.h"
#include "window_rect_t.h"


struct mdc_t
{
	HDC parent;
	HDC handle;
	HBITMAP bmp;
	int w, h;
	
	mdc_t(HDC parent);
	virtual ~mdc_t();
	
	void delete_bmp();
	void delete_dc();
	void resize(int w, int h);
	void resize();
	void flip();
};
