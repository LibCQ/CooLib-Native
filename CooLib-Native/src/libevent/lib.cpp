#include "..\dll\pch.h"
#include "lib.h"
#include "..\dll\dll.h"
#include "..\cqapi\cq.h"
#include "..\cpp-semver\include\cpp-semver.hpp"

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
			cLibInfo i(hlib, path, hlibInfoProc());
			tlibList.push_back(i);
		} while (FindNextFileA(hListFile, &FindFileData));
	}

	// 排序插件
	libList = LibSort(libList);

	// 依次载入
	for (cLibInfo& i : tlibList) {
		if (i.j["ver"] == 1) {
			json bj = json::object();
			bj["s"] = true;
			std::vector<std::string> bmissLib;
			if (!i.j["lib"].empty()) {
				for (auto libName = i.j["require"].begin(); libName != i.j["require"].end(); ++libName) { // 遍历Lib需求表
					std::vector<cLibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), libName.key());
					if (rlib == tlibList.end()) { // 不存在
						cq::CQ_addLog_Error("CooLib-Native", (
							boost::format("插件 %s 依赖的 %s 丢失或不存在，请确认指定插件已经安装并启用。") % i.name % libName.key()
							).str().c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.key());
						continue;
					}
					if (!rlib->loaded) { // 未加载
						cq::CQ_addLog_Error("CooLib-Native", (
							boost::format("插件 %s 依赖的 %s 未正确加载。") % i.name % libName.key()
							).str().c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.key());
						continue;
					}
					if (!versionMatch(rlib->j["AppVer"].get<std::string>(), i.j["require"][libName.key()].get<std::string>())) { // 版本未匹配
						cq::CQ_addLog_Error("CooLib-Native", (
							boost::format("插件 %s 依赖的 %s 版本不正确或版本不合法，请安装正确的版本。(required \"%s\" => installed \"%s\")") % i.name % libName.key() % i.j["require"][libName.key()].get<std::string>() % rlib->j["AppVer"].get<std::string>()
							).str().c_str());
						bj["s"] = false;
						bmissLib.push_back(libName.key());
						continue;
					}
				}
			}
			if (!i.j["using"].empty()) {
				for (auto uname = i.j["require"].begin(); uname != i.j["require"].end(); ++uname) { // 遍历Lib需求表
					std::vector<std::string> i2;
					boost::split(i2, i.j["using"][uname.key()].get<std::string>(), boost::is_any_of("::"));
					if (i2.size() != 2) {
						cq::CQ_addLog_Error("CooLib-Native", (
							boost::format("插件 %s 的 json 中 using 字段的 \"%s\" 不合法。") % i.name % i.j["using"][uname.key()].get<std::string>()
							).str().c_str());
						bj["s"] = false;
						continue;
					}
					std::vector<cLibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), i2[0]);
					for (std::string i3 : bmissLib) { // lib missing
						if (rlib->name == i3) {
							goto nextU;
						}
					}
					if (rlib == tlibList.end()) { // 不存在
						cq::CQ_addLog_Error("CooLib-Native", (
							boost::format("收到来自插件 %s 的依赖请求 %s ，但此依赖并未在 require 字段中注册。") % i.name % uname.key()
							).str().c_str());
						bj["s"] = false;
						bmissLib.push_back(uname.key());
						continue;
					}
					bj["FuncAddr"][uname.key()] = (int32_t)GetProcAddress(rlib->hlib, i2[1].c_str());
				nextU:;
				}
			}
			bj["missLib"] = bmissLib;
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
		json bj = json::object();
		if (!i.j["lib"].empty()) {
			for (auto libName = i.j["require"].begin(); libName != i.j["require"].end(); ++libName) { // 遍历Lib需求表
				std::vector<cLibInfo>::iterator rlib = libFind(libList.begin(), libList.end(), libName.key());
				if (rlib->loaded) { // 已加载
					bloadLib.push_back(rlib->name);
				}
				else { // 未加载
					cq::CQ_addLog_Error("CooLib-Native", (
						boost::format("插件 %s 依赖的 %s 未正确加载。") % i.name % libName.key()
						).str().c_str());
				}
			}
			bj["loadLib"] = bloadLib;
		}
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
	for (const cLibInfo& i : sortLib) {
		cTopological _i;
		_i.name = i.name;
		if (!i.j["require"].empty()) {
			for (auto libName = i.j["require"].begin(); libName != i.j["require"].end(); ++libName) {
				_i.libNames.push_back(libName.key());
			}
		}
		sortT.push_back(_i);
	}
	std::vector<std::string> sortedT = TSort(sortT);
	std::vector<cLibInfo> rl;
	for (std::string i : sortedT) {
		for (const cLibInfo& i2 : sortLib) {
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
					i2 = i.libNames.erase(i2);
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
	return semver::satisfies(version, range);
}
