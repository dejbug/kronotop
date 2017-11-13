#pragma once
#include <stdio.h>
#include <stdarg.h>

#ifdef DEJLIB_DEBUG

#define dbg_out(msg) OutputDebugString(msg)

template<size_t N=512>
void dbg_printf(char const * format, ...) {
	va_list args;
	va_start(args, format);
	char buffer[N+1] = {0};
	_vsnprintf(buffer, N, format, args);
	buffer[N] = 0;
	va_end(args);
	OutputDebugString(buffer);
}

#else

#define dbg_out(msg)

template<size_t N=512>
inline void dbg_printf(char const * format, ...) {}

#endif

