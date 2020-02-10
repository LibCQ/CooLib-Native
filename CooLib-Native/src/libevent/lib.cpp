#include "..\dll\pch.h"
#include "lib.h"
#include "..\dll\dll.h"
#include "..\cqapi\cq.h"

using json = nlohmann::json;

std::vector<cLibInfo> libList; // 插件表

void loadLib() { // 加载Lib
	if (!libList.empty()) unloadLib();
	// 获取DLL路径
	char dllPath[MAX_PATH];
	GetModuleFileNameA(_hModule, dllPath, MAX_PATH);
	PathRemoveFileSpecA(dllPath);
	// 获取CQ路径
	char cqPath[MAX_PATH];
	GetModuleFileNameA(NULL, cqPath, MAX_PATH);
	PathRemoveFileSpecA(cqPath);

	std::vector<cLibInfo> tlibList; // 临时插件表

	// 获取启用插件表
	char cqpConfPath[MAX_PATH];
	lstrcpyA(cqpConfPath, cqPath);
	lstrcatA(cqpConfPath, "\\conf\\cqp.cfg");

	// 遍历载入插件
	char szFilePath[MAX_PATH];
	HANDLE hListFile;
	WIN32_FIND_DATA FindFileData;
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\*.*");
	hListFile = FindFirstFileA(szFilePath, &FindFileData); // 遍历插件
	if (hListFile != INVALID_HANDLE_VALUE) {
		do {
			// 判断插件是否启用
			std::string dllAppID = FindFileData.cFileName;
			dllAppID = dllAppID.substr(0, dllAppID.rfind("."));
			if (dllAppID != "cn.coorg.coolib") { // 有的时候启用事件在ini写入之前
				dllAppID += ".status";
				if (GetPrivateProfileIntA("App", dllAppID.c_str(), 0, cqpConfPath) != 1) continue;
			}
			// 载入插件
			HMODULE hlib;
			std::string path;
			path = path + dllPath + "\\" + FindFileData.cFileName;
			hlib = LoadLibraryA(path.c_str()); // 尝试载入插件
			if (hlib == NULL) { // 不合法
				FreeLibrary(hlib);
				continue;
			}
			typedef const char* (__stdcall* libInfoProc)();
			libInfoProc hlibInfoProc = (libInfoProc)GetProcAddress(hlib, "LibInfo"); // 获取Lib信息
			if (hlibInfoProc == NULL) { // 非Lib
				FreeLibrary(hlib);
				continue;
			}
			json j = json::parse(hlibInfoProc());
			cLibInfo i(hlib, path, j);
			tlibList.push_back(i);
		} while (FindNextFileA(hListFile, &FindFileData));
	}

	// 排序插件
	libList = LibSort(libList);

	// 依次载入
	for (cLibInfo& i : tlibList) {
		if (i.j["ver"] == 1) {
			json bj;
			bj["s"] = true;
			std::vector<std::string> bmissLib;
			std::map<std::string, std::string> libPath;
			if (!i.j["lib"].empty()) {
				for (json::iterator libName = i.j["lib"].begin(); libName != i.j["lib"].end(); ++libName) { // 遍历Lib需求表
					if (libName->empty()) continue;
					std::vector<cLibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), libName.value()["LibAppID"].get<std::string>());
					if (rlib == tlibList.end()) { // 不存在
						std::string errorInfo;
						errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName.value()["LibAppID"].get<std::string>() + " 丢失或不存在，请确认指定插件已经安装并启用。";
						cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.value()["LibAppID"].get<std::string>());
						continue;
					}
					if (!rlib->loaded) { // 未加载
						std::string errorInfo;
						errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName.value()["LibAppID"].get<std::string>() + " 未正确加载。";
						cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.value()["LibAppID"].get<std::string>());
						continue;
					}
					if (!versionMatch(libName.value()["LibVer"].get<std::string>(), rlib->j["AppVer"].get<std::string>())) { // 版本未匹配
						std::string errorInfo;
						errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName.value()["LibAppID"].get<std::string>() + " 版本不正确或版本不合法";
						errorInfo = errorInfo + "（required \"" + libName.value().get<std::string>() + "\" => installed \"" + rlib->j["AppVer"].get<std::string>() + "\"），请安装正确的版本。";
						cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.value()["LibAppID"].get<std::string>());
						continue;
					}
					libPath[rlib->name] = rlib->path;
				}
				if (!bmissLib.empty()) bj["missLib"] = bmissLib;
				bj["libPath"] = libPath;
			}
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
	for (cLibInfo& i : tlibList) {
		if (i.loaded) {
			libList.push_back(i);
		}
		else {
			FreeLibrary(i.hlib); // 释放文件
		}
	}

	// 通知事件
	for (cLibInfo& i : libList) {
		typedef int32_t(__stdcall* appCallbackProc)();
		appCallbackProc happCallbackProc = (appCallbackProc)GetProcAddress(i.hlib, "AppCallback"); // App Callback
		if (happCallbackProc != NULL) {
			appCallbackProc(); // 调用事件
		}
	}

	return;
}

void unloadLib() { // 卸载Lib
	if (libList.empty()) return;

	// 倒序排列
	std::reverse(libList.begin(), libList.end());

	// 通知事件
	for (cLibInfo& i : libList) {
		typedef int32_t(__stdcall* disableCallbackProc)();
		disableCallbackProc hdisableCallbackProc = (disableCallbackProc)GetProcAddress(i.hlib, "DisableCallback"); // App Callback
		if (hdisableCallbackProc != NULL) {
			disableCallbackProc(); // 调用事件
		}
	}

	// 卸载
	for (cLibInfo& i : libList) {
		std::vector<std::string> bloadLib;
		for (json libName : i.j["lib"]) { // 遍历Lib需求表
			if (!libName.empty()) {
				std::vector<cLibInfo>::iterator rlib = libFind(libList.begin(), libList.end(), libName["name"].get<std::string>());
				if (rlib->loaded) { // 已加载
					bloadLib.push_back(rlib->name);
				}
				else { // 未加载
					std::string errorInfo;
					errorInfo = errorInfo + "插件 " + i.name + " 依赖的 " + libName["name"].get<std::string>() + " 未正确加载，请确认插件优先级配置正确（priority字段）。";
					cq::CQ_addLog_Error("CooLib-Native", errorInfo.c_str());
				}
			}
		}
		json bj;
		bj["loadLib"] = bloadLib;
		typedef int32_t(__stdcall* exitCallbackProc)(const char*);
		exitCallbackProc hexitCallbackProc = (exitCallbackProc)GetProcAddress(i.hlib, "ExitCallback"); // App Callback
		if (hexitCallbackProc != NULL) {
			exitCallbackProc(bj.dump().c_str()); // 调用事件
		}
		i.loaded = false;
		FreeLibrary(i.hlib); // 释放插件
	}

	libList.clear(); // 清空表

	return;
}

void reloadLib() { // 重载插件
	if (!libList.empty()) {
		unloadLib();
	}
	loadLib();
	return;
}

std::vector<cLibInfo> LibSort(const std::vector<cLibInfo> sortLib) { // 适配函数
	std::vector<cTopological> sortT;
	for (cLibInfo i : sortLib) {
		cTopological _i;
		_i.name = i.name;
		if (!i.j["lib"].empty()) {
			_i.libNames = i.j["lib"].get<std::vector<std::string>>();
		}
		sortT.push_back(_i);
	}
	std::vector<std::string> sortedT = TSort(sortT);
	std::vector<cLibInfo> rl;
	for (std::string i : sortedT) {
		for (cLibInfo i2 : sortLib) {
			if (i2.name == i) {
				rl.push_back(i2);
				continue;
			}
		}
	}
	return rl;
}

std::vector<std::string> TSort(const std::vector<cTopological> sortT) { // 拓扑排序
	std::vector<cTopological> sortT2 = sortT;
	std::queue<std::string> q;
	for (cTopological i : sortT2) { // 空节点入队
		if (i.libNames.empty()) {
			q.push(i.name);
		}
	}
	std::vector<std::string> rv;
	size_t count = 0;
	while (!q.empty())
	{
		std::string v = q.front(); // 取出一个顶点
		q.pop();
		rv.push_back(v);
		++count;
		// 删除依赖
		for (cTopological& i : sortT2) {
			for (auto i2 = i.libNames.begin(); i2 != i.libNames.end(); ) {
				if (*i2 == v) {
					i2 == i.libNames.erase(i2);
				}
				else {
					i2++;
				}
			}
		}
		// 空节点入队
		for (cTopological i : sortT2) {
			if (i.libNames.empty()) {
				q.push(i.name);
			}
		}
	}
	if (count < sortT.size()) {
		return std::vector<std::string>();
	}
	else {
		return rv;
	}
}

std::vector<cLibInfo>::iterator libFind(std::vector<cLibInfo>::iterator _First, const std::vector<cLibInfo>::iterator _Last, const std::string& _Val) {
	// find first matching _Val
	for (; _First != _Last; ++_First) {
		if (_First->name == _Val) {
			break;
		}
	}
	return _First;
}

bool versionMatch(std::string version, std::string range) {
	// TODO
}
