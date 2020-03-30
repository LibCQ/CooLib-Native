#pragma once
#include "..\dll\pch.h"
#include "cqevent.h"

namespace cq
{
	extern int32_t AuthCode;
	extern HMODULE h;
	extern const char* AppDir;

	typedef int32_t(__stdcall* tCQ_addLog)(int32_t AuthCode, int32_t level, const char* type, const char* info);
	extern tCQ_addLog CQ_addLog;
	typedef int32_t(__stdcall* tCQ_setFatal)(int32_t AuthCode, const char* info);
	extern tCQ_setFatal CQ_setFatal;
	typedef const char* (__stdcall* tCQ_getAppDirectory)(int32_t AuthCode);
	extern tCQ_getAppDirectory CQ_getAppDirectory;

	void __init_api(int32_t _AuthCode);
	void __init_api_startup();

	extern int32_t CQ_addLog_Debug(const char* type, const char* info);
	extern int32_t CQ_addLog_Info(const char* type, const char* info);
	extern int32_t CQ_addLog_Error(const char* type, const char* info);
	extern int32_t CQ_addLog_Fatal(const char* type, const char* info);
}

