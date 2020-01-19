#pragma once

#include "cq.h"
#include "pch.h"

namespace cq {
	extern int32_t CQ_addLog_Debug(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 0, type, info);
	}
	extern int32_t CQ_addLog_Info(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 10, type, info);
	}
	extern int32_t CQ_addLog_Error(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 30, type, info);
	}
	extern int32_t CQ_addLog_Fatal(const char* type, const char* info) {
		return CQ_addLog(cq::AuthCode, 40, type, info);
	}
}
