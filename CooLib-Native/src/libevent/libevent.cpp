#include "..\dll\pch.h"
#include "libevent.h"

extern "C" const char* __stdcall LibInfo() {
	return "{\"ver\":1,\"AppID\":\"cn.coorg.coolib\",\"priority\":0,\"AppVer\":\"1.0.0\"}";
}

extern "C" int32_t __stdcall LibCallback(bool a, const char* b) {
	return 0;
}

extern "C" int32_t __stdcall AppCallback() {
	return 0;
}

extern "C" int32_t __stdcall DisableCallback() {
	return 0;
}

extern "C" int32_t __stdcall ExitCallback(const char* a) {
	return 0;
}

