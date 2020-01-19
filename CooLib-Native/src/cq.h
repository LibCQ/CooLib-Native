#pragma once

#include "pch.h"

namespace cq
{
	extern int32_t AuthCode;
	extern HMODULE h;

	typedef int32_t(__stdcall* tCQ_addLog)(int32_t AuthCode, int32_t level, const char* type, const char* info);
	extern tCQ_addLog CQ_addLog;
	typedef int32_t(__stdcall* tCQ_setFatal)(int32_t AuthCode, const char* info);
	extern tCQ_setFatal CQ_setFatal;

	void __init_api();
}

