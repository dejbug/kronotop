#pragma once
#include <stdio.h>


struct error_t
{
	int code, extra;
	
	error_t(int code=-1);
	error_t(int code, int extra);
	void print();
};


void print_error(const error_t& e);
