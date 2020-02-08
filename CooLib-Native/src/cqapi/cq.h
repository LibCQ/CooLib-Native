#pragma once

#include "..\dll\pch.h"
#include "cq.h"

namespace cq
{
	extern int32_t AuthCode;
	extern HMODULE h;

	typedef int32_t(__stdcall* tCQ_addLog)(int32_t AuthCode, int32_t level, const char* type, const char* info);
	extern tCQ_addLog CQ_addLog;
	typedef int32_t(__stdcall* tCQ_setFatal)(int32_t AuthCode, const char* info);
	extern tCQ_setFatal CQ_setFatal;

	void __init_api();

	extern int32_t CQ_addLog_Debug(const char* type, const char* info);
	extern int32_t CQ_addLog_Info(const char* type, const char* info);
	extern int32_t CQ_addLog_Error(const char* type, const char* info);
	extern int32_t CQ_addLog_Fatal(const char* type, const char* info);
}

