#include <dejlib3/win.h>

namespace dejlib3 {
namespace win {

int run(HWND hwnd, HACCEL haccel)
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
			// throw std::runtime_exception("error in main loop");
			return -1;
		}

		if(hwnd && haccel)
			TranslateAccelerator(hwnd, haccel, &msg);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}

HWND create_window(LPCSTR cname,
		HWND parent, UINT id,
		DWORD style, DWORD xstyle)
{
	HINSTANCE const i = NULL != parent ?
		(HINSTANCE)GetModuleHandle(NULL) :
		(HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE);

	if(NULL == parent) id = 0;
	else style |= WS_CHILD;

	return CreateWindowEx(
		xstyle, cname, "", style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		parent, (HMENU)id, i, NULL);
}

HWND create_window(LPCSTR cname, DWORD style, DWORD xstyle)
{
	return create_window(cname, NULL, 0, style, xstyle);
}

void close_window(HWND h)
{
	SendMessage(h, WM_CLOSE, 0, 0);
}

void repaint_window(HWND h)
{
	InvalidateRect(h, NULL, TRUE);
	UpdateWindow(h);
}

void get_client_size(HWND h, SIZE & size)
{
	RECT r;
	GetClientRect(h, &r);
	size.cx = r.right - r.left;
	size.cy = r.bottom - r.top;
}

Dc Dc::for_client(HWND h)
{
	Dc dc;
	dc.handle = GetDC(h);
	return dc;
}

void Dc::set_pen_color(COLORREF c)
{
	SelectObject(handle, GetStockObject(DC_PEN));
	SetDCPenColor(handle, c);
}

void Dc::set_brush_color(COLORREF c)
{
	SelectObject(handle, GetStockObject(DC_BRUSH));
	SetDCBrushColor(handle, c);
}

PaintDc::PaintDc(HWND h)
{
	handle = BeginPaint(h, &ps);
	SaveDC(ps.hdc);
}

PaintDc::PaintDc(HWND h, COLORREF p, COLORREF b)
{
	handle = BeginPaint(h, &ps);
	SaveDC(ps.hdc);
	set_pen_color(p);
	set_brush_color(b);
}

PaintDc::~PaintDc()
{
	RestoreDC(ps.hdc, -1);
	EndPaint(WindowFromDC(ps.hdc), &ps);
}

void draw_line(HDC dc, POINT a, POINT b)
{
	MoveToEx(dc, a.x, a.y, NULL);
	LineTo(dc, b.x, b.y);
}

}
}
