#pragma once

//#pragma comment(lib,"CQP.lib")
#include "pch.h"

extern "C"
{
	__declspec(dllimport) int32_t CQ_setFatal(int32_t AuthCode, char* Info);
}

