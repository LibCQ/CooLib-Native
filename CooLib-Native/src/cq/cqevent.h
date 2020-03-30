#pragma once
#include "..\dll\pch.h"

extern "C"
{
	const char* __stdcall AppInfo();
	int32_t __stdcall Initialize(int32_t AuthCode);
	int32_t __stdcall _eventStartup();
	int32_t __stdcall _eventExit();
	int32_t __stdcall _eventEnable();
	int32_t __stdcall _eventDisable();
	int32_t __stdcall _configMenu();
	int32_t __stdcall _about();
}

