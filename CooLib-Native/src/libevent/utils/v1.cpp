#include "..\..\dll\pch.h"
#include "..\lib.h"
#include "v1.h"
#include "..\..\dll\dll.h"
#include "..\..\cq\cq.h"
#include "..\..\cpp-semver\include\cpp-semver.hpp"

namespace libutils {
	/*
		通过直接载入dll读取插件
	*/
	std::vector<cLibInfo> loadLib_1() {
		std::vector<cLibInfo> tlibList; // 返回值
		// 获取DLL路径
		char dllPath[MAX_PATH];
		if (GetModuleFileNameA(_hModule, dllPath, MAX_PATH) == 0) throw GetLastError();
		PathRemoveFileSpecA(dllPath);

		/* 读取插件 */
		char szFilePath[MAX_PATH];
		HANDLE hListFile;
		WIN32_FIND_DATA FindFileData;
		lstrcpyA(szFilePath, dllPath);
		lstrcatA(szFilePath, "\\*.*");
		hListFile = FindFirstFileA(szFilePath, &FindFileData); // 遍历插件
		if (hListFile == INVALID_HANDLE_VALUE) return tlibList; // 没有插件可以载入
		do {
			// 插件启用判断
			std::string dllAppID = FindFileData.cFileName;
			dllAppID = dllAppID.substr(0, dllAppID.rfind("."));
			if (!isEnabled(dllAppID)) continue;

			// 对插件执行 LoadLibrary
			HMODULE hlib;
			std::string path;
			path = path + dllPath + "\\" + FindFileData.cFileName;
			// 尝试载入插件
			//auto f = std::async(PLoadLibrary, path.c_str());
			//if (f.wait_for(std::chrono::seconds(15)) != std::future_status::ready) continue; // 插件载入超时 此时酷Q应当已给出载入失败提示
			//hlib = f.get();
			hlib = PLoadLibrary(path.c_str());
			if (hlib == NULL) continue; // 插件不合法 此时酷Q应当已给出载入失败提示

			// 获取插件信息
			typedef const char* (__stdcall* libInfoProc)();
			libInfoProc hlibInfoProc = (libInfoProc)GetProcAddress(hlib, "LibInfo");
			if (hlibInfoProc == NULL) { // 非Lib
				FreeLibrary(hlib);
				continue;
			}
			cLibInfo i(hlib, path, hlibInfoProc()); // 调用并计入变量
			tlibList.push_back(i); // 推入插件组
		} while (FindNextFileA(hListFile, &FindFileData));

		return tlibList;
	}

	HMODULE PLoadLibrary(LPCSTR lpLibFileName) {
		HMODULE hlib;
		try {
			UINT ErrorMode = GetErrorMode();
			SetErrorMode(SEM_NOGPFAULTERRORBOX);
			hlib = LoadLibraryA(lpLibFileName);
			SetErrorMode(ErrorMode);
		}
		catch (...) {
			return 0;
		}
		return hlib;
	}

	/*
		判断插件是否开启
	*/
	bool isEnabled(std::string AppID) {
		// 获取CQ路径
		char cqPath[MAX_PATH];
		if (GetModuleFileNameA(NULL, cqPath, MAX_PATH) == 0) throw GetLastError();
		PathRemoveFileSpecA(cqPath);
		// 获取启用插件表
		char cqConfPath[MAX_PATH];
		lstrcpyA(cqConfPath, cqPath);
		lstrcatA(cqConfPath, "\\conf\\cqp.cfg");

		if (AppID == "cn.coorg.coolib") return true; // 有时启用事件在ini写入之前

		AppID += ".status";
		return GetPrivateProfileIntA("App", AppID.c_str(), 0, cqConfPath) == 1;
	}

	namespace LibCallbackUtils {
		bool LibCallback_1(cLibInfo i, std::vector<cLibInfo> tlibList) {
			json bj = json::object();
			bj["s"] = true;

			// 依赖检查
			std::vector<std::string> bloadedLib;
			if (!i.j["require"].empty()) {
				// 遍历Lib需求表
				for (auto libName = i.j["require"].begin(); libName != i.j["require"].end(); ++libName) {
					// 依赖检测
					auto r = isReqExist(libName.key(), libName.value().get<std::string>(), i.name, tlibList);
					if (r.empty()) {
						bloadedLib.push_back(libName.key());
					}
					else {
						cq::CQ_addLog_Error("CooLib-Native", r.c_str());
						bj["s"] = false;
					}
				}
			}

			// using检查
			if (!i.j["using"].empty()) {
				// 遍历 using 表
				for (auto uname = i.j["using"].begin(); uname != i.j["using"].end(); ++uname) {
					try {
						bj["FuncAddr"][uname.key()] = getUsingAddr(uname.value().get<std::string>(), i.name, tlibList);
					}
					catch (std::string e) {
						cq::CQ_addLog_Error("CooLib-Native", e.c_str());
						bj["s"] = false;
					}
				}
			}
			bj["loadedLib"] = bloadedLib;

			// 返回详情并记录
			typedef int32_t(__stdcall* libCallbackProc)(bool, const char*);
			libCallbackProc hlibCallbackProc = (libCallbackProc)GetProcAddress(i.hlib, "LibCallback"); // Lib Callback
			if (hlibCallbackProc == NULL) return false;
			auto i2 = hlibCallbackProc(bj["s"].get<bool>(), bj.dump(-1, ' ', true).c_str());
			return i2 == 0;
		}

		/*
			依赖检测
		*/
		std::string isReqExist(std::string libAppID, std::string versionRange, std::string AppID, std::vector<cLibInfo> tlibList) {
			/* 查找对应依赖 */
			std::vector<cLibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), libAppID);

			// 依赖不存在
			if (rlib == tlibList.end() || !rlib->loaded) {
				return (boost::format("插件 %s 依赖的 %s 丢失或不存在，请确认指定插件已经安装并启用。") % AppID % libAppID).str();
			}
			// 依赖版本未匹配
			if (!versionMatch(rlib->j["AppVer"].get<std::string>(), versionRange)) {
				return (boost::format("插件 %s 依赖的 %s 版本不正确或版本不合法，请安装正确的版本。(required \"%s\" => installed \"%s\")") % AppID % libAppID % versionRange % rlib->j["AppVer"].get<std::string>()).str();
			}

			return "";
		}

		int32_t getUsingAddr(std::string usingProc, std::string AppID, std::vector<cLibInfo> tlibList) {
			std::vector<std::string> i;

			// 分割作用域符
			boost::split_regex(i, usingProc, boost::regex("::"));
			// 没有作用域，声明不合法
			if (i.size() < 2) {
				throw (boost::format("插件 %s 的 json 中 using 字段的 \"%s\" 不合法。") % usingProc).str();
			}

			//// 在依赖表查找对应插件
			//auto clib = std::find(tlibList->begin(), tlibList->end(), i[0]);
			//// 插件未被依赖或未载入
			//if (clib == tlibList->end()) {
			//	throw (boost::format("收到来自插件 %s 的依赖请求 %s ，但此依赖并未加载，可能依赖并未正常安装和载入或依赖未在 require 字段中注册。") % AppID % usingProc).str();
			//}

			// 在依赖数据表查找对应插件
			std::vector<cLibInfo>::iterator rlib = libFind(tlibList.begin(), tlibList.end(), i[0]);
			if (rlib == tlibList.end() || !rlib->loaded) {
				throw (boost::format("收到来自插件 %s 的依赖请求 %s ，但此依赖并未加载，可能依赖并未正常安装和载入或依赖未在 require 字段中注册。") % AppID % usingProc).str();
			}

			getUsingAddrUtils::getUsingAddr_1(i[1], rlib->j["LibAPI"], rlib->hlib);

			return (int32_t)GetProcAddress(rlib->hlib, i[1].c_str());
		}

		namespace getUsingAddrUtils {
			int32_t getUsingAddr_1(std::string usingProc, json iJson, HMODULE hlib) {
				std::vector<std::string> i;
				boost::split_regex(i, usingProc, boost::regex("::"));
				if (i.size() > 1) {
					for (auto ProcN = iJson.begin(); ProcN != iJson.end(); ++ProcN) {
						if (ProcN.value().is_object()) {
							if (ProcN.key() == i[0]) {
								int32_t i = getUsingAddr_1(usingProc, ProcN.value(), hlib);
								if (i != 0) return i;
								continue;
							}
						}
					}
				}
				else if (i.size() == 1) {
					for (auto ProcN = iJson.begin(); ProcN != iJson.end(); ++ProcN) {
						if (ProcN.value().is_string()) {
							if (ProcN.key() == usingProc) {
								return (int32_t)GetProcAddress(hlib, ProcN.value().get<std::string>().c_str());
							}
						}
					}
				}
				return 0;
			}
		}
	}

	namespace appCallbackUtils {
		int32_t appCallback_1(HMODULE hlib) {
			typedef int32_t(__stdcall* appCallbackProc)();
			appCallbackProc happCallbackProc = (appCallbackProc)GetProcAddress(hlib, "AppCallback"); // App Callback
			if (happCallbackProc == NULL) return 0;
			// 调用事件
			return happCallbackProc();
		}
	}
}