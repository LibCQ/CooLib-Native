#include "..\dll\pch.h"
#include "libevent.h"
#include "lib.h"

extern "C" const char* __stdcall LibInfo() {
return (const char *)R"(
{
    "ver": 1,
    "AppID": "cn.coorg.coolib",
    "AppVer": "1.0.0",
    "lib": [],
    "LibAPI": [
        {
            "func": "CheckLibA",
            "?": "@@YG_NPADPAD@Z"
        },
        {
            "func": "VersionMatch",
            "?": "@@YG_NPADPAD@Z"
        }
    ]
}
)";
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

extern "C" int32_t __stdcall CheckLibA(const char* LibAppID, const char* LibVer) {
	for (cLibInfo& i : libList) {
		if (i.name == LibAppID && versionMatch(LibVer, i.j["AppVer"].get<std::string>())) {
			return true;
		}
	}
	return false;
}

extern "C" int32_t __stdcall VersionMatch(const char* version, const char* range) {
	return versionMatch(version, range);
}
