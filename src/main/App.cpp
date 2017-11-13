#include "App.h"
// #include <main/globals.h>
#include <main/resource.h>
#include <dejlib3/win.h>
#include <snippets/tests.h>

#include <stdio.h>
#include <Scintilla.h>
// #include <SciLexer.h>

#include <dejlib3/dbg.h>

using Sci = dejlib3::sci::Sci;

LRESULT App::smsg(UINT m, WPARAM w, LPARAM l)
{
	return sci.smsg(m, w, l);
}

void App::create_child_controls()
{
	hwnd_editor = sci.create(hwnd_main, IDC_EDITOR);
	sci.set_defaults();
	// assert(NULL == wndproc_editor_def);
	// wndproc_editor_def = (WNDPROC)SetWindowLong(hwnd_editor, GWL_WNDPROC, (LONG)EditorProc);
	sci.subclass(EditorProc);

	sci.show();
	SetFocus(hwnd_editor);
}

void App::layout_child_controls()
{
	SIZE s;

	win::get_client_size(hwnd_main, s);
	// printf("cr: %ld %ld\n", s.cx, s.cy);

	// window_positioner_t wp(hwnd_editor);
	// wp.setpos(0, 0);
	// wp.setsize(s.cx, s.cy);

	// MoveWindow(hwnd_editor, 0, 0, s.cx, s.cy, TRUE);
	MoveWindow(hwnd_editor,
		sides, 0, s.cx - sides*2, s.cy, TRUE);
}

#define DISABLE_SCI(m) case m: return 0;
#define DISABLE_SCI_START(m) case m: {
#define DISABLE_SCI_END(ret) return ret; }
#define ALLOW_SCI_IF(expr) if (expr) break;

LRESULT CALLBACK App::EditorProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	static bool unpainted_resize = true;

	auto sci = Sci::from_handle(h);

	// char const * const sci_str = sci_code_to_str(m);

	switch(m)
	{
		default:
		{
			// if (sci_str) printf("subclass event %s | %08x %08x\n", sci_str, (unsigned) w, (unsigned) l);
			// else printf("subclass event %x (%d) | %08x %08x\n", m, m, (unsigned) w, (unsigned) l);
			break;
		}

		case WM_SIZE:
		{
			unpainted_resize = true;
			InvalidateRect(h, NULL, FALSE);
			break;
		}

		case WM_PAINT:
		{
			if (unpainted_resize)
				unpainted_resize = false;
			else
			{
				// std::vector<long> yy;
				// sci.get_last_wrapped_rows_y(yy);
				// sci.clip_row_separators(yy);

				// std::vector<long> xx;
				// sci.get_row_widths(xx);
				// sci.clip_col_separators(xx);
			}

			LRESULT const res = sci.calldef(m, w, l);

			// sci.draw_row_separators(yy);
			// sci.draw_col_separators(xx);

			return res;
		}

		case WM_RBUTTONDOWN:
		{
			return 0;
		}

		DISABLE_SCI(SCI_SETWRAPMODE)
		DISABLE_SCI(SCI_SETMARGINWIDTHN)

		DISABLE_SCI_START(SCI_SETWRAPSTARTINDENT)
			printf("* currently no other wrap indents supported: only 0\n");
		DISABLE_SCI_END(0)

		DISABLE_SCI_START(SCI_SETWRAPVISUALFLAGS)
			ALLOW_SCI_IF(SC_WRAPVISUALFLAG_NONE == w)
			ALLOW_SCI_IF(SC_WRAPVISUALFLAG_END == w)
			printf("* invalid wrap visual flag: only NONE or END\n");
		DISABLE_SCI_END(0)
	}

	return sci.calldef(m, w, l);
}

WNDPROC App::wndproc_editor_def = NULL;
