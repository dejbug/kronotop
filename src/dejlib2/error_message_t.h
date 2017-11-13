#pragma once
#include <windows.h>


struct error_message_t
{
	char * handle;

	error_message_t(DWORD winerror, bool in_english_please=false)
	:	handle(NULL)
	{
		DWORD const lid_def = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
		DWORD const lid_en = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

		DWORD result = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			winerror,
			in_english_please ? lid_en : lid_def,
			(LPTSTR)&handle,
			0,
			NULL
		);

		if(!result)
			handle = strdup("<error while formatting error message>");
	}

	virtual ~error_message_t()
	{
		if(handle)
			delete[] handle;
		handle = NULL;
	}
};
