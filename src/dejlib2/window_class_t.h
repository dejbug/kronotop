#pragma once
#include <windows.h>

#include "errors.h"
#include "error_t.h"


struct window_class_t : public WNDCLASSEX
{
	window_class_t(LPCSTR name, WNDPROC callback=DefWindowProc);
	
	void zeromemory();
	void init();
	void install();
	void uninstall();
	void load();
};
