#include "SciSelectionDragDisabler.h"
#include <Scintilla.h>
// #include <SciLexer.h>

SciSelectionDragDisabler::SciSelectionDragDisabler(HWND h, bool is_active) : sci(Sci::from_handle(h)), is_active(is_active)
{}

SciSelectionDragDisabler::SciSelectionDragDisabler(Sci & sci, bool is_active) : sci(Sci::from_handle(sci.handle)), is_active(is_active)
{}

bool SciSelectionDragDisabler::on_down(int x, int y)
{
	if (!is_active) return false;

	last_ldown_pos = sci.smsg(SCI_POSITIONFROMPOINT, x, y);

	if (last_ldown_pos >= sci.smsg(SCI_GETSELECTIONSTART) && last_ldown_pos <= sci.smsg(SCI_GETSELECTIONEND))
	{
		sci.smsg(SCI_GOTOPOS, last_ldown_pos);
		was_selection_cleared = true;
		last_ldown_time.update();
	}
	else
		was_selection_cleared = false;

	return was_selection_cleared;
}

bool SciSelectionDragDisabler::on_ddown(int x, int y)
{
	if (!is_active) return false;

	last_ldown_pos = sci.smsg(SCI_POSITIONFROMPOINT, x, y);
	return on_ddown();
}

bool SciSelectionDragDisabler::on_ddown()
{
	if (!is_active) return false;

	if (was_selection_cleared && last_ldown_time.delta() > max_dclick_time)
	{
		was_selection_cleared = false;
		if (last_ldown_pos != sci.smsg(SCI_WORDSTARTPOSITION, last_ldown_pos, true))
			sci.smsg(SCI_WORDLEFT);
		sci.smsg(SCI_WORDRIGHTEXTEND);
		return true;
	}

	return false;
}
