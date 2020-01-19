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
}
