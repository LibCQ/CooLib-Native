#pragma once

extern "C"
{
	const char* __stdcall LibInfo();
	int32_t __stdcall LibCallback(bool a, const char* b);
	int32_t __stdcall AppCallback();
	int32_t __stdcall DisableCallback();
	int32_t __stdcall ExitCallback(const char* a);
}