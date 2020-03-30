#include "..\dll\pch.h"
#include "cq.h"

namespace cq {
	int32_t AuthCode;
	HMODULE h;
	const char* AppDir;

	tCQ_addLog CQ_addLog;
	tCQ_setFatal CQ_setFatal;
	tCQ_getAppDirectory CQ_getAppDirectory;

	void __init_api(int32_t _AuthCode) {
		AuthCode = _AuthCode;
		h = GetModuleHandleA("CQP.dll");
		if (h != NULL) {
			CQ_addLog = reinterpret_cast<tCQ_addLog>(GetProcAddress(h, "CQ_addLog"));
			CQ_setFatal = reinterpret_cast<tCQ_setFatal>(GetProcAddress(h, "CQ_setFatal"));
			CQ_getAppDirectory = reinterpret_cast<tCQ_getAppDirectory>(GetProcAddress(h, "CQ_getAppDirectory"));
		}
	}

	void __init_api_startup() {
		if (CQ_getAppDirectory != NULL) {
			AppDir = CQ_getAppDirectory(AuthCode);
		}
	}

	int32_t CQ_addLog_Debug(const char* type, const char* info) {
		return CQ_addLog(AuthCode, 0, type, info);
	}
	int32_t CQ_addLog_Info(const char* type, const char* info) {
		return CQ_addLog(AuthCode, 10, type, info);
	}
	int32_t CQ_addLog_Error(const char* type, const char* info) {
		return CQ_addLog(AuthCode, 30, type, info);
	}
	int32_t CQ_addLog_Fatal(const char* type, const char* info) {
		return CQ_addLog(AuthCode, 40, type, info);
	}
}
