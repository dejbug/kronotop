#pragma once
#include "rect_t.h"


struct grid_t
{
	rect_t bounds;
	int cols, rows;
	int cw, ch, gap;
	
	grid_t();
	
	void setup(const rect_t& r, int cols, int rows, int gap=0);
	void reset();
	bool are_neighbors(int src, int dst) const;
};
