#pragma once
#include <windows.h>
#include "window_class_t.h"


struct hinstance_t
{
	HINSTANCE handle;

	hinstance_t();
	hinstance_t(HINSTANCE handle);
	hinstance_t(window_class_t& wc);
	hinstance_t(HWND h);
};
