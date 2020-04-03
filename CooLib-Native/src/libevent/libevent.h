#pragma once

extern "C"
{
	const char* __stdcall LibInfo();
	int32_t __stdcall LibCallback(bool a, const char* b);
	int32_t __stdcall AppCallback();
	int32_t __stdcall DisableCallback();
	int32_t __stdcall ExitCallback(const char* a);
	int32_t __stdcall CheckLibA(const char* LibAppID, const char* LibVer);
	int32_t __stdcall VersionMatch(const char* ver1, const char* ver2);
	bool __stdcall IsEnabled(const char* AppID);
	bool __stdcall LibLoaded(const char* a);
	extern "C" bool __stdcall GetLibVersion(const char* AppID, char* VersionBack, size_t BufferSize);
}
