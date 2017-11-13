#pragma once
#include <windows.h>
#include <zmouse.h>
#include <vector>
#include "listeners.h"
#include "dnd_tracker_t.h"


struct mouse_tracker_move_i
{
	virtual void on_mouse_move(int x, int y) = 0;
};

struct mouse_tracker_wheel_i
{
	virtual void on_mouse_wheel(int x, int y,
		int delta, unsigned int keys) = 0;
};

struct mouse_tracker_click_i
{
	virtual void on_mouse_button(int x, int y,
		int b, int state, unsigned int keys) = 0;
};

struct mouse_tracker_click2_i
{
	virtual void on_mouse_button_down(int x, int y,
		int b, unsigned int keys) = 0;

	virtual void on_mouse_button_up(int x, int y,
		int b, unsigned int keys) = 0;
};


struct mouse_tracker_t
{
	std::vector<mouse_tracker_move_i*> move;
	std::vector<mouse_tracker_wheel_i*> wheel;
	std::vector<mouse_tracker_click_i*> click;
	std::vector<mouse_tracker_click2_i*> click2;

	void callback(HWND h, UINT m, WPARAM w, LPARAM l);
};
