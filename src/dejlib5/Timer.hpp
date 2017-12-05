#ifndef KRONOTOPE_TIMER_HPP
#define KRONOTOPE_TIMER_HPP

#include <windows.h>

struct Timer
{
	DWORD msec = 0;

	inline Timer() : msec(GetTickCount()) { }

	inline DWORD update() { return msec = GetTickCount(); }
	inline DWORD delta() const { return GetTickCount() - msec; }
	inline DWORD delta_sec() const { return delta() / 1000; }
};

#endif // !KRONOTOPE_TIMER_HPP
