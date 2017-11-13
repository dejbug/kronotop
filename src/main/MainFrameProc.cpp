#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <stdio.h>

#include <Scintilla.h>

#include <dejlib2/error_t.h>
#include <dejlib2/win.h>
#include <dejlib3/win.h>
#include <dejlib3/dbg.h>

#include <main/globals.h>
#include <main/App.h>

#include <snippets/tests.h>

App app;

bool wm_create(HWND h, LPCREATESTRUCT cs)
{
	DragAcceptFiles(h, TRUE);

	app.hwnd_main = h;
	app.create_child_controls();

	// SetTimer(h, 4711, 250, NULL);
	return true;
}

void wm_activate(HWND h, UINT f, HWND prev, BOOL minimized)
{
	if(WA_ACTIVE == f || WA_CLICKACTIVE == f)
		SetFocus(app.hwnd_editor);
}

void wm_timer(HWND h, UINT id)
{
}

void wm_size(HWND h, UINT type, short width, short height)
{
	app.layout_child_controls();
}

void wm_paint(HWND h)
{
	COLORREF const b = app.smsg(SCI_STYLEGETBACK);
	COLORREF const f = app.smsg(SCI_STYLEGETFORE);

	dejlib3::win::PaintDc dc(h, b, b);

	SIZE s;
	win::get_client_size(h, s);

	Rectangle(dc.handle, 0, 0, app.sides, s.cy);
	Rectangle(dc.handle, s.cx-app.sides, 0, s.cx, s.cy);

	dc.set_pen_color(f);

	dejlib3::win::draw_line(dc.handle,
		{app.sides-1, 0},
		{app.sides-1, s.cy});

	dejlib3::win::draw_line(dc.handle,
		{s.cx-app.sides+1, 0},
		{s.cx-app.sides+1, s.cy});
}

void wm_keydown(HWND h, UINT key, BOOL, int repeatCount, UINT flags)
{
	switch(key)
	{
		case VK_ESCAPE:
			win::close_window(h);
			break;

		case VK_F5:
			InvalidateRect(h, NULL, TRUE);
			UpdateWindow(h);
			break;
	}
}

void wm_command(HWND h, int id, HWND ctrl, UINT code)
{
	/// -- handle accelerators.
	if(1 == code) switch(id)
	{
		case IDM_ESCAPE:
			win::close_window(h);
			break;

		case IDM_F8:
			app.sci.smsg(SCI_SETWRAPVISUALFLAGS, app.sci.smsg(SCI_GETWRAPVISUALFLAGS) ? SC_WRAPVISUALFLAG_NONE :  SC_WRAPVISUALFLAG_END);
			break;
	}
}

void on_sci_notify(HWND h, SCNotification * info)
{
	// std::string s;
	// app.sci.print_notification_label(s, info->nmhdr.code);
	// printf("[%d] %s\n", info->nmhdr.code, s.c_str());

	if (SCN_PAINTED == info->nmhdr.code)
	{
		std::vector<long> yy;
		app.sci.get_last_wrapped_rows_y(yy);
		app.sci.draw_row_separators(yy);
		// DUMP_VEC("ld", yy);

		std::vector<long> xx;
		app.sci.get_row_widths(xx);
		app.sci.draw_col_separators(xx);
		// DUMP_VEC("ld", xx);

		// app.sci.draw_cursor();
	}
}

LRESULT CALLBACK MainFrameProc(HWND h, UINT m, WPARAM wParam, LPARAM lParam)
{
	try {
		switch(m)
		{
			default: break;

			HANDLE_MSG(h, WM_CREATE, wm_create);
			HANDLE_MSG(h, WM_ACTIVATE, wm_activate);
			HANDLE_MSG(h, WM_SIZE, wm_size);
			HANDLE_MSG(h, WM_PAINT, wm_paint);
			HANDLE_MSG(h, WM_TIMER, wm_timer);
			HANDLE_MSG(h, WM_KEYDOWN, wm_keydown);
			HANDLE_MSG(h, WM_COMMAND, wm_command);

			case WM_CLOSE: DestroyWindow(h); return 0;
			case WM_DESTROY: PostQuitMessage(0); return 0;
			case WM_ERASEBKGND: return 0;

			case WM_NOTIFY:
			{
				if(IDC_EDITOR == wParam) on_sci_notify(h, reinterpret_cast<SCNotification *>(lParam));
				return 0;
			}
		}
	}

	catch(error_t& e) {
		e.print();
	}

	return DefWindowProc(h, m, wParam, lParam);
}
