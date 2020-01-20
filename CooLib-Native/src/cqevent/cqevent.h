#pragma once

extern "C"
{
	const char* __stdcall AppInfo();
	int32_t __stdcall Initialize(int32_t AuthCode);
	int32_t __stdcall _eventStartup();
	int32_t __stdcall _eventExit();
}

