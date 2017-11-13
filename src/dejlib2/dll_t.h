#pragma once
#include <windows.h>
#include "error_t.h"


struct dll_t
{
	HINSTANCE handle;
	bool autofree;

	dll_t(char const * fpath, bool autofree=false);
	virtual ~dll_t();

	void free();
};
