#pragma once
#include "..\lib.h"

namespace libutils {
	std::vector<cLibInfo> loadLib_1();
	HMODULE PLoadLibrary(LPCSTR lpLibFileName);
	namespace LibCallbackUtils {
		bool LibCallback_1(cLibInfo i, std::vector<cLibInfo> tlibList);
		std::string isReqExist(std::string libAppID, std::string versionRange, std::string AppID, std::vector<cLibInfo> tlibList);
		int32_t getUsingAddr(std::string usingProc, std::string AppID, std::vector<cLibInfo> tlibList);
		namespace getUsingAddrUtils {
			int32_t getUsingAddr_1(std::string usingProc, json iJson, HMODULE hlib);
		}
	}
	namespace appCallbackUtils {
		int32_t appCallback_1(HMODULE hlib);
	}
}
