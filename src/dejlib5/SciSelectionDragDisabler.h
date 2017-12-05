#ifndef KRONOTOPE_SCI_SELECTION_DRAG_DISABLER_H
#define KRONOTOPE_SCI_SELECTION_DRAG_DISABLER_H

#include <dejlib3/sci.h>
#include "Timer.hpp"

using Sci = dejlib3::sci::Sci;

struct SciSelectionDragDisabler
{
	Sci sci;
	bool is_active = true;
	UINT const max_dclick_time = GetDoubleClickTime();
	bool was_selection_cleared = false;
	int last_ldown_pos = 0;
	Timer last_ldown_time;

	SciSelectionDragDisabler(HWND h, bool is_active=true);
	SciSelectionDragDisabler(Sci & sci, bool is_active=true);

	bool on_down(int x, int y);
	bool on_ddown(int x, int y);
	bool on_ddown();
};

#endif // !KRONOTOPE_SCI_SELECTION_DRAG_DISABLER_H
