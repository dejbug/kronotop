#pragma once
#include <windows.h>
#include <vector>


struct rect_t
{
	RECT rect;
	long width, height;
	long &left, &top, &right, &bottom;
	long &l, &t, &r, &b;
	long &x, &y, &w, &h;
	
	typedef std::vector<rect_t> vector;
	
	rect_t(long x=0, long y=0, long w=0, long h=0);
	rect_t(const rect_t& o);

	rect_t& operator=(const rect_t& o);
	
	void offset(long dx, long dy);
	void inflate(long dw, long dh);
	bool contains(int x, int y) const;
};


void draw_rect(HDC hdc, const rect_t& r, int radius=0);

