#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "error_t.h"
#include "listeners.h"


struct dnd_tracker_drops_i
{
	virtual void on_drop(int x, int y,
		std::vector<std::string>& names) = 0;
};


struct dnd_tracker_t
{
	std::vector<dnd_tracker_drops_i*> listeners;
	std::vector<std::string> names;
	
	void callback(HWND h, UINT m, WPARAM w, LPARAM l);
};
