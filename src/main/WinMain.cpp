#include <windows.h>

#include <dejlib2/dll_t.h>
#include <dejlib2/win.h>
#include <dejlib2/window_class_t.h>
#include <dejlib2/window_maker_t.h>
#include <dejlib2/window_positioner_t.h>

extern LRESULT CALLBACK MainFrameProc(HWND, UINT, WPARAM, LPARAM);

int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	try
	{
		dll_t("SciLexer.dll");

		HACCEL haccel = LoadAccelerators(instance, "accel_main");

		window_class_t wc("MAINFRAME", MainFrameProc);
		wc.install();

		window_maker_t wm(wc);
		wm.create();

		SetWindowText(wm.handle, "skeletype");

		window_positioner_t wp(wm.handle);
		wp.setsize(782, 400);
		wp.center_to_screen();

		UpdateWindow(wm.handle);
		ShowWindow(wm.handle, SW_SHOW);

		return win::run(wm.handle, haccel);
	}

	catch(error_t& e)
	{
		e.print();
	}

	return -1;
}
