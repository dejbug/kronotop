#pragma once
#include <windows.h>


namespace win {

	int run(HWND hwnd=NULL, HACCEL haccel=NULL);
	void close_window(HWND h);
	void repaint_window(HWND h);
	void get_client_size(HWND h, SIZE& size);

} //namespace win
