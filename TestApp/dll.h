#pragma once
#include "pch.h"
#include <cstdint>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

typedef int32_t(__stdcall* tCQ_addLog)(int32_t AuthCode, int32_t level, const char* type, const char* info);
typedef int32_t(__stdcall* tAdd)(int32_t a, int32_t b);

extern "C" const char* __stdcall AppInfo();
extern "C" int32_t __stdcall Initialize(int32_t _AuthCode);
extern "C" int32_t __stdcall _eventStartup();
extern "C" int32_t __stdcall _eventEnable();
extern "C" int32_t __stdcall _eventDisable();
extern "C" int32_t __stdcall _eventExit();
extern "C" const char* __stdcall LibInfo();
extern "C" int32_t __stdcall LibCallback(bool a, const char* b);
extern "C" int32_t __stdcall AppCallback();
extern "C" int32_t __stdcall DisableCallback();
extern "C" int32_t __stdcall ExitCallback(const char* a);

