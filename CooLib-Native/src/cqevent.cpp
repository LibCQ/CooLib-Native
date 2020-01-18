// cqevent.cpp 定义CQ事件
#include "pch.h"

#include "dll.h"
#include "cqevent.h"

using json = nlohmann::json;

__declspec(dllexport) const char* AppInfo()
{
	return "9,cn.coorg.coolib";
}

__declspec(dllexport) int32_t Initialize(int32_t AuthCode)
{

	return 0;
}

__declspec(dllexport) int32_t _eventStartup()
{
	// 获取DLL路径
	char dllPath[MAX_PATH];
	GetModuleFileNameA(_hModule, dllPath, MAX_PATH);
	PathRemoveFileSpecA(dllPath);
	// 获取酷Q路径
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	PathRemoveFileSpecA(exePath);

	bool fail = false; // 崩溃性错误
	json missLib; // 丢失Lib表

	char szFilePath[MAX_PATH];
	HANDLE hListFile;
	WIN32_FIND_DATA FindFileData;
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\*.*");
	hListFile = FindFirstFileA(szFilePath, &FindFileData); // 遍历插件
	if (hListFile != INVALID_HANDLE_VALUE) {
		do {
			HMODULE hlib;
			hlib = LoadLibraryA(FindFileData.cFileName); // 尝试载入插件
			if (hlib) {
				typedef const char* (__stdcall* libInfoProc)();
				libInfoProc hlibInfoProc = (libInfoProc)GetProcAddress(hlib, "LibInfo"); // 获取Lib信息
				if (hlibInfoProc) {
					json j, bj;
					j = json::parse(hlibInfoProc());
					if (j["ver"] == 1) {
						for (std::string i : j["lib"]) { // 遍历Lib需求表
							if (!i.empty()) {
								std::string slibPath = exePath;
								slibPath += "\\";
								slibPath += i;
								slibPath += ".dll";
								if (PathFileExists(slibPath.c_str())) { // Lib是否存在
									bj["libPath"][i] = slibPath;
								}
								else {
									bool fail = true;
									missLib[FindFileData.cFileName].push_back(i);
									bj["s"] = false;
									bj["missLib"].push_back(i);
								}
							}
						}
					}
					typedef int32_t(__stdcall* libCallbackProc)(const char*);
					libCallbackProc hlibCallbackProc = (libCallbackProc)GetProcAddress(hlib, "LibCallback"); // Lib Callback
					if (hlibCallbackProc) {
						hlibCallbackProc(bj.dump().c_str()); // 返回详情
					}
				}
				FreeLibrary(hlib); // 释放文件
			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}


	return 0;
}