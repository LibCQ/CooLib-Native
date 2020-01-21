// cqevent.cpp 定义CQ事件
#include "..\dll\pch.h"

#include "..\dll\dll.h"
#include "..\libevent\lib.h"
#include "cqevent.h"
#include "..\cqapi\cq.h"

extern "C" const char* __stdcall AppInfo()
{
	return "9,cn.coorg.coolib";
}

extern "C" int32_t __stdcall Initialize(int32_t AuthCode)
{
	cq::AuthCode = AuthCode;
	cq::__init_api();
	return 0;
}

extern "C" int32_t __stdcall _eventStartup()
{

	return 0;
}

extern "C" int32_t __stdcall _eventEnable() {

	LARGE_INTEGER time;
	QueryPerformanceFrequency(&time);
	LONGLONG timeDff = time.QuadPart;
	QueryPerformanceCounter(&time);
	__int64 timeStart = time.QuadPart;

	loadLib();

	QueryPerformanceCounter(&time);
	__int64 timeEnd = time.QuadPart;
	std::string debugInfo;
	debugInfo = debugInfo + "Loaded.(" + std::to_string(libList.size()) + " succeed, " + std::to_string((double)(timeEnd - timeStart) * 1000 / timeDff) + "ms)";
	cq::CQ_addLog_Debug("CooLib-Native", debugInfo.c_str());

	return 0;
}

extern "C" int32_t __stdcall _eventDisable() {

	//LARGE_INTEGER time;
	//QueryPerformanceFrequency(&time);
	//LONGLONG timeDff = time.QuadPart;
	//QueryPerformanceCounter(&time);
	//__int64 timeStart = time.QuadPart;

	unloadLib();

	//QueryPerformanceCounter(&time);
	//__int64 timeEnd = time.QuadPart;
	//std::string debugInfo;
	//debugInfo = debugInfo + "Unloaded.(" + std::to_string(libList.size()) + " succeed, " + std::to_string((double)(timeEnd - timeStart) * 1000 / timeDff) + "ms)";
	//cq::CQ_addLog_Debug("CooLib-Native", debugInfo.c_str());
	// -997

	return 0;
}

extern "C" int32_t __stdcall _eventExit()
{

	return 0;
}
