#pragma once
#include <windows.h>


struct temp_object_t
{
	HGDIOBJ handle;
	
	temp_object_t(HGDIOBJ handle=NULL);
	virtual ~temp_object_t();
	
	temp_object_t(const temp_object_t&);
	temp_object_t& operator=(const temp_object_t&);
};
