#include "..\dll\pch.h"
#include "libevent.h"
#include "lib.h"

extern "C" const char* __stdcall LibInfo() { // 在这里填写应用相关信息
return (const char *)R"(
{
    "ver": 1,
    "AppID": "cn.coorg.coolib",
    "priority": 0,
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

extern "C" int32_t __stdcall LibCallback(bool a, const char* b) { // 库载入事件，在这里初始化所有API接口
	return 0;
}

extern "C" int32_t __stdcall AppCallback() { // 应用载入事件，在这里初始化应用
	return 0;
}

extern "C" int32_t __stdcall DisableCallback() { // 应用卸载事件，在这里结束应用
	return 0;
}

extern "C" int32_t __stdcall ExitCallback(const char* a) { // 库卸载事件，在这里结束库
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
