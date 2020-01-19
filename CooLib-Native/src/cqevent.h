#pragma once

extern "C"
{
	const char* __stdcall AppInfo();
	int32_t __stdcall Initialize(int32_t AuthCode);
	int32_t __stdcall _eventStartup();
	int32_t __stdcall _eventExit();
	const char* __stdcall LibInfo();
	int32_t __stdcall LibCallback(bool a, const char* b);
	int32_t __stdcall AppCallback();
	int32_t __stdcall DisableCallback();
	int32_t __stdcall ExitCallback(const char* a);
}

