#pragma once
#include <windows.h>
#include <string>

#include "errors.h"
#include "error_t.h"
#include "window_class_t.h"


struct window_maker_t
{
	HWND handle;
	HINSTANCE hinstance;
	std::string clsname;
	
	window_maker_t(window_class_t& wc);
	window_maker_t(char const * clsname, HINSTANCE=NULL);
	
	HWND create();
	HWND create(HWND parent, UINT id);
};
