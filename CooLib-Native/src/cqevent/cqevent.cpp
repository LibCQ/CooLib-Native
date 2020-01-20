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

	clock_t timeStart = clock();

	loadLib();

	clock_t timeEnd = clock();
	std::string debugInfo;
	debugInfo = debugInfo + "Loaded.(" + std::to_string(libList.size()) + " succeed, " + std::to_string(timeEnd - timeStart) + "ms)";
	cq::CQ_addLog_Debug("CooLib-Native", debugInfo.c_str());

	return 0;
}

extern "C" int32_t __stdcall _eventDisable() {

	clock_t timeStart = clock();

	unloadLib();

	clock_t timeEnd = clock();
	std::string debugInfo;
	debugInfo = debugInfo + "Unloaded.(" + std::to_string(libList.size()) + " succeed, " + std::to_string(timeEnd - timeStart) + "ms)";
	cq::CQ_addLog_Debug("CooLib-Native", debugInfo.c_str());

	return 0;
}

extern "C" int32_t __stdcall _eventExit()
{

	return 0;
}
