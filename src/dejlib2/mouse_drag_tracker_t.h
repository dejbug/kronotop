#pragma once
#include <windows.h>
#include <vector>
#include "mkeys_t.h"
#include "listeners.h"


struct mouse_drag_i
{
	virtual void on_mouse_drag_start(int x, int y,
		int b, unsigned int keys) = 0;
	virtual void on_mouse_drag_end(int x, int y,
		int b, unsigned int keys) = 0;
	virtual void on_mouse_drag_move(int x, int y,
		int b, unsigned int keys) = 0;
	virtual void on_mouse_drag_lost(int b) = 0;
};


struct mouse_drag_tracker_t
{
	std::vector<mouse_drag_i*> drag;

	int dragging_button;

	mouse_drag_tracker_t();

	void callback(HWND h, UINT m, WPARAM w, LPARAM l);
};
