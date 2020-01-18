#pragma once

extern "C"
{
	__declspec(dllexport) const char* AppInfo();
	__declspec(dllexport) int32_t Initialize(int32_t AuthCode);
	__declspec(dllexport) int32_t _eventStartup();
}

