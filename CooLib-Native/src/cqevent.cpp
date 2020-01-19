// cqevent.cpp 定义CQ事件
#include "pch.h"

#include "dll.h"
#include "cqevent.h"
#include "cqapi.h"

using json = nlohmann::json;

std::vector<LibInfo> libList; // 插件表

const char* AppInfo()
{
	return "9,cn.coorg.coolib";
}

int32_t Initialize(int32_t AuthCode)
{
	cq::AuthCode = AuthCode;
	cq::__init_api();
	return 0;
}

int32_t _eventStartup()
{
	// 获取DLL路径
	char dllPath[MAX_PATH];
	GetModuleFileNameA(_hModule, dllPath, MAX_PATH);
	PathRemoveFileSpecA(dllPath);

	std::vector<LibInfo> tlibList; // 临时插件表

	// 遍历载入插件
	char szFilePath[MAX_PATH];
	HANDLE hListFile;
	WIN32_FIND_DATA FindFileData;
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\*.*");
	hListFile = FindFirstFileA(szFilePath, &FindFileData); // 遍历插件
	if (hListFile != INVALID_HANDLE_VALUE) {
		do {
			HMODULE hlib;
			std::string path;
			path = path + dllPath + "\\" + FindFileData.cFileName;
			hlib = LoadLibraryA(path.c_str()); // 尝试载入插件
			if (hlib != NULL) {
				typedef const char* (__stdcall* libInfoProc)();
				libInfoProc hlibInfoProc = (libInfoProc)GetProcAddress(hlib, "LibInfo"); // 获取Lib信息
				if (hlibInfoProc != NULL) {
					json j = json::parse(hlibInfoProc());
					LibInfo i(hlib, path, j);
					tlibList.push_back(i);
				}
			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}

	// 排序插件
	std::sort(tlibList.begin(), tlibList.end(), upsort);

	// 依次载入
	for (LibInfo i : tlibList) {
		if (i.j["ver"] == 1) {
			json bj = json::parse("{\"s\":true}");
			std::vector<std::string> bmissLib;
			std::map<std::string, std::string> libPath;
			for (std::string libName : i.j["lib"]) { // 遍历Lib需求表
				if (!libName.empty()) {
					std::vector<LibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), libName);
					if (rlib != tlibList.end()) { // 存在
						if (rlib->loaded) { // 已加载
							libPath[rlib->name] = rlib->path;
						}
						else { // 未加载
							std::string errorInfo;
							errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName + " 未正确加载，请确认插件优先级配置正确（priority字段）。";
							cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
							bj["s"] = false;
							bmissLib.push_back(libName);
						}
					}
					else { // 不存在
						std::string errorInfo;
						errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName + " 丢失或不存在，请确认指定插件已经安装并启用。";
						cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
						bj["s"] = false;
						bmissLib.push_back(libName);
					}
				}
			}
			if (!bmissLib.empty()) bj["missLib"] = bmissLib;
			bj["libPath"] = libPath;
			typedef int32_t(__stdcall* libCallbackProc)(bool, const char*);
			libCallbackProc hlibCallbackProc = (libCallbackProc)GetProcAddress(i.hlib, "LibCallback"); // Lib Callback
			if (hlibCallbackProc != NULL) {
				if (hlibCallbackProc(bmissLib.empty(), bj.dump().c_str()) == 0) { // 返回详情
					i.loaded = true;
				}
			}
		}
	}

	// 存盘
	for (LibInfo i : tlibList) {
		if (i.loaded) {
			libList.push_back(i);
		}
		else {
			FreeLibrary(i.hlib); // 释放文件
		}
	}

	return 0;
}

int32_t _eventEnable() {

	// 通知事件
	for (LibInfo i : libList) {
		typedef int32_t(__stdcall* appCallbackProc)(bool, const char*);
		appCallbackProc happCallbackProc = (appCallbackProc)GetProcAddress(i.hlib, "AppCallback"); // App Callback
		if (happCallbackProc != NULL) {
			appCallbackProc(); // 调用事件
		}
	}

	return 0;
}

int32_t _eventDisable() {

	// 倒序排列
	std::sort(libList.begin(), libList.end(), downsort);

	// 通知事件
	for (LibInfo i : libList) {
		typedef int32_t(__stdcall* disableCallbackProc)(bool, const char*);
		disableCallbackProc hdisableCallbackProc = (disableCallbackProc)GetProcAddress(i.hlib, "DisableCallback"); // App Callback
		if (hdisableCallbackProc != NULL) {
			disableCallbackProc(); // 调用事件
		}
	}

	return 0;
}

int32_t _eventExit()
{
	// 通知事件
	for (LibInfo i : libList) {
		std::vector<std::string> bloadLib;
		for (std::string libName : i.j["lib"]) { // 遍历Lib需求表
			if (!libName.empty()) {
				std::vector<LibInfo>::iterator rlib = libFind(libList.begin(), libList.end(), libName);
				if (rlib->loaded) { // 已加载
					bloadLib.push_back(rlib->name);
				}
				else { // 未加载
					std::string errorInfo;
					errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName + " 未正确加载，请确认插件优先级配置正确（priority字段）。";
					cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
				}
			}
		}
		json bj;
		bj["loadLib"] = bloadLib;
		typedef int32_t(__stdcall* disableCallbackProc)(const char*);
		disableCallbackProc hdisableCallbackProc = (disableCallbackProc)GetProcAddress(i.hlib, "DisableCallback"); // App Callback
		if (hdisableCallbackProc != NULL) {
			disableCallbackProc(bj.dump().c_str()); // 调用事件
		}
	}

	for (LibInfo i : libList) {
		FreeLibrary(i.hlib); // 释放文件
	}

	return 0;
}

bool upsort(LibInfo i, LibInfo j) {
	return i.priority < j.priority;
}

bool downsort(LibInfo i, LibInfo j) {
	return i.priority > j.priority;
}

std::vector<LibInfo>::iterator libFind(std::vector<LibInfo>::iterator _First, const std::vector<LibInfo>::iterator _Last, const std::string& _Val) {
	// find first matching _Val
	for (; _First != _Last; ++_First) {
		if (_First->name == _Val) {
			break;
		}
	}
	return _First;
}
