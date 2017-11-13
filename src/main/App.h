#pragma once
#include <dejlib3/sci.h>
#include <vector>

struct App
{
	dejlib3::sci::Sci sci;

	HWND hwnd_main = NULL;
	HWND hwnd_editor = NULL;
	int sides = 32;

	LRESULT smsg(UINT m, WPARAM w=0, LPARAM l=0);

	void create_child_controls();

	void layout_child_controls();

private:
	static WNDPROC wndproc_editor_def;

	static LRESULT CALLBACK EditorProc(HWND h, UINT m, WPARAM w, LPARAM l);

};

char const * sci_code_to_str(long code);

