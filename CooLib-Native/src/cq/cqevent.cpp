// cqevent.cpp 定义CQ事件
#include "..\dll\pch.h"

#include "..\dll\dll.h"
#include "..\libevent\lib.h"
#include "cq.h"
#include "cqevent.h"

extern "C" const char* __stdcall AppInfo()
{
	return "9,cn.coorg.coolib";
}

extern "C" int32_t __stdcall Initialize(int32_t AuthCode)
{
	cq::__init_api(AuthCode);
	return 0;
}

extern "C" int32_t __stdcall _eventStartup()
{
	cq::__init_api_startup();

	// 通知 CooLib
	typedef bool(__stdcall* tLibLoaded)(const char* a);
	char dllPath[MAX_PATH];
	if (GetModuleFileNameA(_hModule, dllPath, MAX_PATH) == 0) return 0;
	PathRemoveFileSpecA(dllPath);
	char szFilePath[MAX_PATH];
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\cn.coorg.coolib.cpk");
	HMODULE hcl = LoadLibraryA(szFilePath);
	if (hcl == 0) {
		lstrcpyA(szFilePath, dllPath);
		lstrcatA(szFilePath, "\\cn.coorg.coolib.dll");
		hcl = LoadLibraryA(szFilePath);
		if (hcl == 0) return 0;
	}
	tLibLoaded LibLoaded;
	DWORD l;
	LibLoaded = reinterpret_cast<tLibLoaded>(GetProcAddress(hcl, "LibLoaded"));
	if (LibLoaded) {
		LibLoaded("cn.coorg.coolib");
	}
	// 请保持 Coolib 加载，*不要*调用 FreeLibrary

	return 0;
}

extern "C" int32_t __stdcall _eventEnable() {

	cq::CQ_addLog_Info("Loader", "CooLib 仍在加载中，在加载完毕前消息都将被丢弃");

	libutils::hCQThreadExitEvent = CreateEventA(NULL, true, false, NULL);
	libutils::hCQThread = CreateThread(NULL, 0, libutils::CQThreadProc, NULL, 0, NULL);

	return 0;
}

extern "C" int32_t __stdcall _eventDisable() {

	//LARGE_INTEGER time;
	//QueryPerformanceFrequency(&time);
	//LONGLONG timeDff = time.QuadPart;
	//QueryPerformanceCounter(&time);
	//__int64 timeStart = time.QuadPart;

	if (libutils::hCQThread && libutils::hCQThreadExitEvent) {
		SetEvent(libutils::hCQThreadExitEvent);
		if (WaitForSingleObject(libutils::hCQThread, 5 * 1000) == WAIT_TIMEOUT) {
			TerminateThread(libutils::hCQThread, 0);
			if (libutils::hCQThreadEvent) CloseHandle(libutils::hCQThreadEvent);
			libutils::hCQThreadEvent = 0;
			if (libutils::hCQThreadExitEvent) CloseHandle(libutils::hCQThreadExitEvent);
			libutils::hCQThreadExitEvent = 0;
		}
		std::queue<std::string> emptyQueue;
		if (!libutils::loadedAppIDList.empty()) std::swap(emptyQueue, libutils::loadedAppIDList);
	}

	libutils::unloadLib();

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

//extern "C" int32_t __stdcall _configMenu()
//{
//	return 0;
//}

extern "C" int32_t __stdcall _about()
{

	return 0;
}
