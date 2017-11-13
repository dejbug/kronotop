
#include <windows.h>
#include <shellapi.h>

#include "clipper_t.h"
#include "color_t.h"
#include "debug.h"
#include "dll_t.h"
#include "dnd_tracker_t.h"
#include "error_t.h"
#include "errors.h"
#include "grid_t.h"
#include "hinstance_t.h"
#include "listeners.h"
#include "mdc_t.h"
#include "mdc2_t.h"
#include "mkeys_t.h"
#include "mouse_drag_tracker_t.h"
#include "mouse_info_t.h"
#include "mouse_tracker_t.h"
#include "palettes.h"
#include "rect_t.h"
#include "temp_color_setter_t.h"
#include "temp_hdc_t.h"
#include "temp_object_t.h"
#include "win.h"
#include "window_class_t.h"
#include "window_maker_t.h"
#include "window_positioner_t.h"
#include "window_rect_t.h"


clipper_t::clipper_t(HDC hdc)
{
	this->hdc = hdc;
}

void clipper_t::clear()
{
	SelectClipRgn(hdc, NULL);
}

void clipper_t::clipper_t::fill()
{
	HWND h = WindowFromDC(hdc);
	window_rect_t r(h);
	
	temp_object_t reg(
		CreateRectRgn(r.l, r.t, r.r, r.b));
	
	SelectClipRgn(hdc, (HRGN)reg.handle);
}

void clipper_t::clipper_t::merge(int x, int y, int w, int h)
{
	temp_object_t reg(CreateRectRgn(x, y, x+w, y+h));
	ExtSelectClipRgn(hdc, (HRGN)reg.handle, RGN_OR);
}

void clipper_t::cut(int x, int y, int w, int h)
{
	ExcludeClipRect(hdc, x, y, x+w, y+h);
}

void clipper_t::crop(int x, int y, int w, int h)
{
	IntersectClipRect(hdc, x, y, x+w, y+h);
}


// #include "color_t.h"


color_t::color_t(COLORREF c) : c(c)
{
}

color_t::color_t(char r, char g, char b) :
		c(RGB(r,g,b))
{
}

color_t::color_t(int h, bool soft, bool bright) :
		c(hsv(float(h),
		soft ? 0.33f : 0.66f,
		bright ? 0.88f : 0.55f))
{
}

color_t::color_t(int h, float s, float v) :
		c(hsv(float(h),s,v))
{
}

color_t::color_t(int h, double s, double v) :
		c(hsv(float(h),s,v))
{
}

void color_t::print()
{
	printf("cref %08lx (%ld,%ld,%ld)\n",
		c, c&0xff, (c>>8)&0xff, (c>>16)&0xff);
}


// [ source:] <https://www.cs.rit.edu/~ncs/color/t_convert.html>
COLORREF hsv(float h, float s, float v)
{
	int i;
	float r,g,b;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		//~ r = g = b = v;
		float c = 255*v;
		return RGB(c, c, c);
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
	
	return RGB(r*255, g*255, b*255);
}


// #include "dll_t.h"


dll_t::dll_t(char const * fpath, bool autofree)
:	autofree(autofree)
{
	handle = LoadLibrary(fpath);
	if(!handle)
		throw error_t(err::DLL_LOAD);
}

dll_t::~dll_t()
{
	if(autofree)
		free();
}

void dll_t::free()
{
	if(handle)
		FreeLibrary(handle);
	handle = NULL;
}


// #include "dnd_tracker_t.h"


void dnd_tracker_t::callback(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch(m)
	{
		case WM_DROPFILES:
		{
			HDROP const hDrop = (HDROP)w;
			
			POINT pt = {0,0};
			DragQueryPoint(hDrop, &pt);
			
			UINT const file_count =
				DragQueryFile(hDrop, (UINT)-1, NULL, 0);
			
			char name[MAX_PATH] = {0};
			
			names.clear();
			
			for(UINT i=0; i<file_count; ++i)
			{
				UINT const file_name_length =
					DragQueryFile(hDrop, i, NULL, 0);
					
				UINT const chars_copied =
					DragQueryFile(hDrop, i,
						name, sizeof(name));
						
				/// this should never happen.
				if(chars_copied != file_name_length)
					error_t(OUT_OF_BUFFER);
			
				names.push_back(name);
			}
			
			DragFinish(hDrop);
			
			NOTIFY_LISTENERS(listeners)->
				on_drop(pt.x, pt.y, names);
			
			break;
		}
	}
}


// #include "error_t.h"


error_t::error_t(int code) :
	code(code), extra(GetLastError())
{}

error_t::error_t(int code, int extra) :
	code(code), extra(extra)
{}

void error_t::print()
{
	print_error(*this);
}

void print_error(const error_t& e)
{
	printf("! error[%d] : %08x (%d)\n",
		e.code, e.extra, e.extra);
}


// #include "grid_t.h"


grid_t::grid_t()
{
	reset();
}

void grid_t::setup(const rect_t& r, int cols, int rows, int gap)
{
	reset();
	
	cw = r.w / cols;
	ch = r.h / rows;
	
	if(cw <= 0 || ch <= 0)
		throw error_t(err::ARGS);
	
	bounds = r;
	this->cols = cols;
	this->rows = rows;
	this->gap = gap;
}

void grid_t::reset()
{
	bounds = rect_t();
	cols = rows = gap = 0;
	cw = ch = 0;
}

bool grid_t::are_neighbors(int src, int dst) const
{
	return (
		src + 1 == dst ||
		src - 1 == dst ||
		src + cols == dst ||
		src - cols == dst
	);
}


// #include "mdc2_t.h"


mdc2_t::mdc2_t(HDC hdc) :
	mdc_t(hdc)
{}

void mdc2_t::clear(UINT id)
{
	window_rect_t r(handle, true);
	FillRect(handle, &r.rect, (HBRUSH)(id+1));
}

void mdc2_t::clear(color_t c)
{
	temp_color_setter_t cs(handle);
	cs.fb(false, true);
	cs.bg(c.c);
	
	window_rect_t r(handle, true);
	//~ FillRect(handle, &r.rect, (HBRUSH)(id+1));
	Rectangle(handle, r.l, r.t, r.r, r.b);
}


// #include "hinstance_t.h"


hinstance_t::hinstance_t()
{
	handle = (HINSTANCE)GetModuleHandle(NULL);
}

hinstance_t::hinstance_t(HINSTANCE handle)
:	handle(handle)
{}

hinstance_t::hinstance_t(window_class_t& wc)
:	handle(wc.hInstance)
{}

hinstance_t::hinstance_t(HWND h)
{
	handle = (HINSTANCE)GetWindowLong(h, GWL_HINSTANCE);
}


// #include "mdc_t.h"


mdc_t::mdc_t(HDC parent) :
	parent(parent), handle(NULL), bmp(NULL)
{
	if(!parent) return;
	
	handle = CreateCompatibleDC(parent);
	if(!handle) throw error_t(err::MDC);
	
	resize();
}

mdc_t::~mdc_t()
{
	delete_bmp();
	delete_dc();
}

void mdc_t::delete_bmp()
{
	if(handle) SelectObject(handle, (HBITMAP)NULL);
	if(bmp) { DeleteObject(bmp); bmp = NULL; }
}

void mdc_t::delete_dc()
{
	if(!handle) return;
	DeleteDC(handle);
	handle = NULL;
}

void mdc_t::resize(int w, int h)
{
	if(!handle) return;
	if(bmp) delete_bmp();
	
	bmp = CreateCompatibleBitmap(parent, w, h);
	if(!bmp) throw error_t(err::MDC_BMP);

	SelectObject(handle, (HBITMAP)bmp);

	this->w = w;
	this->h = h;
}

void mdc_t::resize()
{
	window_rect_t r(parent, true);
	resize(r.w, r.h);
}

void mdc_t::flip()
{
	BitBlt(parent, 0, 0, w, h, handle, 0, 0, SRCCOPY);
}


// #include "mkeys_t.h"


mkeys_t::mkeys_t(DWORD w)
:	w(w)
{
}

bool mkeys_t::is_button_down() const
{
	return w & MK_BUTTON_MASK;
}

int mkeys_t::get_button_down_count() const
{
	return 0 +
		(w & MK_LBUTTON ? 1 : 0) +
		(w & MK_MBUTTON ? 1 : 0) +
		(w & MK_RBUTTON ? 1 : 0);
}

int mkeys_t::get_button_up_count() const
{
	return 3 -
		(w & MK_LBUTTON ? 1 : 0) +
		(w & MK_MBUTTON ? 1 : 0) +
		(w & MK_RBUTTON ? 1 : 0);
}

int mkeys_t::get_main_button_down_lmr() const
{
	if(w & MK_LBUTTON) return 1;
	if(w & MK_MBUTTON) return 2;
	if(w & MK_RBUTTON) return 3;
	return 0;
}

int mkeys_t::get_main_button_down_lrm() const
{
	if(w & MK_LBUTTON) return 1;
	if(w & MK_RBUTTON) return 3;
	if(w & MK_MBUTTON) return 2;
	return 0;
}

int mkeys_t::get_main_button_down_mlr() const
{
	if(w & MK_MBUTTON) return 2;
	if(w & MK_LBUTTON) return 1;
	if(w & MK_RBUTTON) return 3;
	return 0;
}

int mkeys_t::get_main_button_down_mrl() const
{
	if(w & MK_MBUTTON) return 2;
	if(w & MK_RBUTTON) return 3;
	if(w & MK_LBUTTON) return 1;
	return 0;
}

int mkeys_t::get_main_button_down_rlm() const
{
	if(w & MK_RBUTTON) return 3;
	if(w & MK_LBUTTON) return 1;
	if(w & MK_MBUTTON) return 2;
	return 0;
}

int mkeys_t::get_main_button_down_rml() const
{
	if(w & MK_RBUTTON) return 3;
	if(w & MK_MBUTTON) return 2;
	if(w & MK_LBUTTON) return 1;
	return 0;
}

bool mkeys_t::is_left_button_down() const
{
	return w & MK_LBUTTON;
}

bool mkeys_t::is_only_left_button_down() const
{
	return MK_LBUTTON == (w & MK_BUTTON_MASK);
}

bool mkeys_t::is_middle_button_down() const
{
	return w & MK_MBUTTON;
}

bool mkeys_t::is_only_middle_button_down() const
{
	return MK_MBUTTON == (w & MK_BUTTON_MASK);
}

bool mkeys_t::is_right_button_down() const
{
	return w & MK_RBUTTON;
}

bool mkeys_t::is_only_right_button_down() const
{
	return MK_RBUTTON == (w & MK_BUTTON_MASK);
}

bool mkeys_t::is_control_down() const
{
	return w & MK_CONTROL;
}

bool mkeys_t::is_shift_down() const
{
	return w & MK_SHIFT;
}


// #include "mouse_drag_tracker_t.h"


mouse_drag_tracker_t::mouse_drag_tracker_t()
{
	dragging_button = 0;
}

void mouse_drag_tracker_t::callback(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch(m)
	{
		case WM_MOUSEMOVE:
		{
			const unsigned int keys = w;
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			if(dragging_button)
				NOTIFY_LISTENERS(drag)
					->on_mouse_drag_move(x, y, dragging_button, keys);

		}	break;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			const unsigned int keys = w;
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			dragging_button = mkeys_t(keys).get_main_button_down_lrm();

			SetCapture(h);

			NOTIFY_LISTENERS(drag)
				->on_mouse_drag_start(x, y, dragging_button, keys);

		}	break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			const unsigned int keys = w;
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			if(GetCapture() == h)
				ReleaseCapture();

			if(dragging_button)
			{
				NOTIFY_LISTENERS(drag)
					->on_mouse_drag_end(x, y, dragging_button, keys);

				dragging_button = 0;
			}

		}	break;

		case WM_CAPTURECHANGED:
		{
			// const HWND lost_to = (HWND)l;

			if(dragging_button)
			{
				NOTIFY_LISTENERS(drag)
					->on_mouse_drag_lost(dragging_button);

				dragging_button = 0;
			}

		}	break;
	}
}


// #include "mouse_info_t.h"


bool mouse_info_t::has_mouse()
{
	return GetSystemMetrics(SM_MOUSEPRESENT);
}

bool mouse_info_t::has_wheel()
{
	return GetSystemMetrics(SM_MOUSEWHEELPRESENT);
}

bool mouse_info_t::buttons_swapped()
{
	return GetSystemMetrics(SM_SWAPBUTTON);
}

int mouse_info_t::get_buttons_count()
{
	return GetSystemMetrics(SM_CMOUSEBUTTONS);
}


// #include "mouse_tracker_t.h"


void mouse_tracker_t::callback(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch(m)
	{
		case WM_MOUSEMOVE:
		{
			// const unsigned int keys = w;
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(move)
				->on_mouse_move(x, y);

		}	break;

		case WM_MOUSEWHEEL:
		{
			const unsigned int keys = LOWORD(w);
			const short delta = HIWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(wheel)
				->on_mouse_wheel(x, y, delta, keys);

		}	break;

		case WM_LBUTTONDOWN:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 1, 1, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_down(x, y, 1, keys);

		}	break;

		case WM_LBUTTONUP:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 1, 0, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_up(x, y, 1, keys);

		}	break;

		case WM_MBUTTONDOWN:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 2, 1, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_down(x, y, 2, keys);

		}	break;

		case WM_MBUTTONUP:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 2, 0, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_up(x, y, 2, keys);

		}	break;

		case WM_RBUTTONDOWN:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 3, 1, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_down(x, y, 3, keys);

		}	break;

		case WM_RBUTTONUP:
		{
			const unsigned int keys = LOWORD(w);
			const int x = LOWORD(l);
			const int y = HIWORD(l);

			NOTIFY_LISTENERS(click)
				->on_mouse_button(x, y, 3, 0, keys);

			NOTIFY_LISTENERS(click2)
				->on_mouse_button_up(x, y, 3, keys);

		}	break;
	}
}


// #include "rect_t.h"


rect_t::rect_t(long x, long y, long w, long h) :
	width(w), height(h),
	left(rect.left), top(rect.top),
	right(rect.right), bottom(rect.bottom),
	l(left), t(top), r(right), b(bottom),
	x(left), y(top), w(width), h(height)
{
	left = x;
	top = y;
	right = x+w;
	bottom = y+h;
}

rect_t::rect_t(const rect_t& o)
:	width(o.w), height(o.h),
	left(rect.left), top(rect.top),
	right(rect.right), bottom(rect.bottom),
	l(left), t(top), r(right), b(bottom),
	x(left), y(top), w(width), h(height)
{
	left = o.l;
	top = o.t;
	right = o.r;
	bottom = o.b;
}

rect_t& rect_t::operator=(const rect_t& o)
{
	left = o.l;
	top = o.t;
	right = o.r;
	bottom = o.b;
	width = o.width;
	height = o.height;
	
	return *this;
}

void rect_t::offset(long dx, long dy)
{
	l += dx;
	r += dx;
	t += dy;
	b += dy;
}

void rect_t::inflate(long dw, long dh)
{
	l -= dw;
	r += dw;
	t -= dh;
	b += dh;
	w = r-l;
	h = b-t;
}

bool rect_t::contains(int x, int y) const
{
	return x >= l && x <= r && y >= t && y <= b;
}

void draw_rect(HDC hdc, const rect_t& r, int radius)
{
	RoundRect(hdc, r.l, r.t, r.r+1, r.b+1, radius, radius);
}


// #include "temp_color_setter_t.h"


temp_color_setter_t::temp_color_setter_t(HDC hdc) :
	hdc(hdc),
	null_pen(CreatePen(PS_NULL, 0, 0))
{
	old_brush = SelectObject(hdc,
		(HBRUSH)GetStockObject(DC_BRUSH));
	
	old_pen = SelectObject(hdc,
		(HPEN)GetStockObject(DC_PEN));
}

temp_color_setter_t::~temp_color_setter_t()
{
	reset();
	
	if(null_pen) DeleteObject(null_pen);
}

void temp_color_setter_t::reset()
{
	SelectObject(hdc, old_brush);
	SelectObject(hdc, old_pen);
}

void temp_color_setter_t::fb(bool fg, bool bg)
{
	ff(fg);
	bb(bg);
}

void temp_color_setter_t::ff(bool fg, bool bg)
{
	if(fg)
		SelectObject(hdc, (HPEN)GetStockObject(DC_PEN));
	else
		SelectObject(hdc, null_pen);
}

void temp_color_setter_t::bb(bool bg, bool fg)
{
	if(bg)
		SelectObject(hdc, (HBRUSH)GetStockObject(DC_BRUSH));
	else
		SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
}

void temp_color_setter_t::fg(char r, char g, char b)
{
	SetDCPenColor(hdc, RGB(r,g,b));
}

void temp_color_setter_t::bg(char r, char g, char b)
{
	SetDCBrushColor(hdc, RGB(r,g,b));
}

void temp_color_setter_t::fg(COLORREF c)
{
	SetDCPenColor(hdc, c);
}

void temp_color_setter_t::bg(COLORREF c)
{
	SetDCBrushColor(hdc, c);
}


// #include "temp_hdc_t.h"


temp_hdc_t::temp_hdc_t(HWND parent) :
	parent(parent),
	handle(GetDC(parent))
{
}

temp_hdc_t::~temp_hdc_t()
{
	if(handle)
		ReleaseDC(parent, handle);
}


// #include "temp_object_t.h"


temp_object_t::temp_object_t(HGDIOBJ handle) :
		handle(handle)
{
}

temp_object_t::~temp_object_t()
{
	if(handle)
	{
		DeleteObject(handle);
		handle = NULL;
	}
}


// #include "win.h"


int win::run(HWND hwnd, HACCEL haccel)
{
	MSG msg;
	
	while(1)
	{
		int res = GetMessage(&msg, NULL, 0, 0);
		
		if(0 == res)
		{
			// WM_QUIT
			break;
		}
		
		else if(res < 0)
		{
			// errors!
			throw error_t(err::MESSAGE_LOOP);
		}

		if(hwnd && haccel)
			TranslateAccelerator(hwnd, haccel, &msg);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}


void win::close_window(HWND h)
{
	SendMessage(h, WM_CLOSE, 0, 0);
}

void win::repaint_window(HWND h)
{
	InvalidateRect(h, NULL, TRUE);
	UpdateWindow(h);
}

void win::get_client_size(HWND h, SIZE& size)
{
	RECT r;
	GetClientRect(h, &r);
	size.cx = r.right - r.left;
	size.cy = r.bottom - r.top;
}
	
// #include "window_class_t.h"


window_class_t::window_class_t(LPCSTR name, WNDPROC callback)
{
	init();
	
	lpszClassName = name;
	lpfnWndProc = callback;
}

void window_class_t::zeromemory()
{
	memset(this, 0, sizeof(WNDCLASSEX));
}

void window_class_t::init()
{
	cbSize = sizeof(WNDCLASSEX);
	
	cbClsExtra = 0;
	cbWndExtra = 0;
	
	style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	
	hInstance = GetModuleHandle(NULL);
	hIcon = hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	hCursor = LoadCursor(NULL, IDC_ARROW);
	hbrBackground = (HBRUSH)COLOR_WINDOW;
	
	lpszMenuName = NULL;
	lpszClassName = NULL;
	lpfnWndProc = DefWindowProc;
}

void window_class_t::install()
{
	if(!RegisterClassEx(this))
		throw error_t(err::WINDOW_CLASS_INSTALL);
}

void window_class_t::uninstall()
{
	if(!UnregisterClass(lpszClassName, hInstance))
		throw error_t(err::WINDOW_CLASS_UNINSTALL);
}

void window_class_t::load()
{
	if(!GetClassInfoEx(hInstance, lpszClassName, this))
		throw error_t(err::WINDOW_CLASS_LOAD);
}


// #include "window_maker_t.h"


window_maker_t::window_maker_t(window_class_t& wc) :
	handle(NULL),
	hinstance(wc.hInstance),
	clsname(wc.lpszClassName)
{
	if(!hinstance)
		hinstance = GetModuleHandle(NULL);
}

window_maker_t::window_maker_t(char const * clsname, HINSTANCE hinstance) :
	handle(NULL),
	hinstance(hinstance),
	clsname(clsname)
{
}

HWND window_maker_t::create()
{
	handle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW|WS_EX_ACCEPTFILES|
			WS_EX_CONTEXTHELP|WS_EX_CONTROLPARENT,
		clsname.c_str(),
		"",
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hinstance,
		NULL
	);
	
	if(!handle)
		throw error_t(err::WINDOW_MAKER_CREATE);
	
	return handle;
}

HWND window_maker_t::create(HWND parent, UINT id)
{
	handle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		clsname.c_str(),
		"",
		WS_CHILD|WS_CLIPSIBLINGS|WS_TABSTOP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		parent,
		(HMENU)id,
		hinstance_t(parent).handle,
		NULL
	);
	
	if(!handle)
		throw error_t(err::WINDOW_MAKER_CREATE);
	
	return handle;
}


// #include "window_positioner_t.h"


window_positioner_t::window_positioner_t(HWND handle) :
	handle(handle), extra_flags(SWP_NOACTIVATE)
{
	if(!handle)
		throw error_t(err::WINDOW_POSITIONER_HANDLE);
}

void window_positioner_t::move(int dx, int dy) const
{
	window_rect_t r(handle);
	
	SetWindowPos(handle, NULL, r.left+dx, r.top+dy, 0, 0,
		extra_flags|SWP_NOZORDER|SWP_NOSIZE);
}

void window_positioner_t::setpos(int x, int y) const
{
	SetWindowPos(handle, NULL, x, y, 0, 0,
		extra_flags|SWP_NOZORDER|SWP_NOSIZE);
}

void window_positioner_t::setsize(int w, int h) const
{
	SetWindowPos(handle, NULL, 0, 0, w, h,
		extra_flags|SWP_NOZORDER|SWP_NOMOVE);
}

void window_positioner_t::below(HWND other) const
{
	SetWindowPos(handle, other, 0, 0, 0, 0,
		extra_flags|SWP_NOMOVE|SWP_NOSIZE);
}

void window_positioner_t::to_bottom() const
{
	SetWindowPos(handle, HWND_BOTTOM, 0, 0, 0, 0,
		extra_flags|SWP_NOMOVE|SWP_NOSIZE);
}

void window_positioner_t::to_top() const
{
	SetWindowPos(handle, HWND_TOP, 0, 0, 0, 0,
		extra_flags|SWP_NOMOVE|SWP_NOSIZE);
}

void window_positioner_t::to_permanent_top() const
{
	SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0,
		extra_flags|SWP_NOMOVE|SWP_NOSIZE);
}

void window_positioner_t::to_quasi_top() const
{
	SetWindowPos(handle, HWND_NOTOPMOST, 0, 0, 0, 0,
		extra_flags|SWP_NOMOVE|SWP_NOSIZE);
}

void window_positioner_t::center_to_screen() const
{
	window_rect_t c(handle);
	window_rect_t p(GetParent(handle));
	
	const int dx = (p.width - c.width) >> 1;
	const int dy = (p.height - c.height) >> 1;
	
	setpos(p.left + dx, p.top + dy);
}


// #include "window_rect_t.h"


window_rect_t::window_rect_t(HWND parent, bool client) :
	parent(parent)
{
	update(client);
}

window_rect_t::window_rect_t(HDC hdc, bool client) :
	parent(WindowFromDC(hdc))
{
	update(client);
}

void window_rect_t::update(bool client)
{
	if(!parent)
	{
		left = top = 0;
		right = GetSystemMetrics(SM_CXSCREEN);
		bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		BOOL(WINAPI *method_f)(HWND,LPRECT) =
			client ? GetClientRect : GetWindowRect;
		
		method_f(parent, &rect);
	}
	
	width = right - left;
	height = bottom - top;
}

SIZE window_rect_t::size() const
{
	const SIZE s = {w, h};
	return s;
}

POINT window_rect_t::pos() const
{
	const POINT p = {x, y};
	return p;
}

