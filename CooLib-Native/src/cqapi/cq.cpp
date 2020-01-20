#include "..\dll\pch.h"
#include "cq.h"

namespace cq {
	int32_t AuthCode;
	HMODULE h;

	tCQ_addLog CQ_addLog;
	tCQ_setFatal CQ_setFatal;

	void __init_api() {
		h = GetModuleHandleA("CQP.dll");
		if (h != NULL) {
			CQ_addLog = reinterpret_cast<tCQ_addLog>(GetProcAddress(h, "CQ_addLog"));
			CQ_setFatal = reinterpret_cast<tCQ_setFatal>(GetProcAddress(h, "CQ_setFatal"));
		}
	}

	int32_t CQ_addLog_Debug(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 0, type, info);
	}
	int32_t CQ_addLog_Info(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 10, type, info);
	}
	int32_t CQ_addLog_Error(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 30, type, info);
	}
	int32_t CQ_addLog_Fatal(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 40, type, info);
	}
}
