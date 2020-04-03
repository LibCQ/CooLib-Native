#include "..\dll\pch.h"
#include "libevent.h"
#include "lib.h"

extern "C" const char* __stdcall LibInfo() {
	return (const char*)R"(
{
    "ver": 1,
    "AppID": "cn.coorg.coolib",
    "AppVer": "0.0.1",
    "require": {},
    "using": {},
    "LibAPI": {
        "library": {
            "CheckLibA": "CheckLibA",
            "VersionMatch": "VersionMatch",
            "IsEnabled": "IsEnabled",
			"GetVersion": "GetLibVersion"
        }
    }
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

extern "C" bool __stdcall LibLoaded(const char* a) {
	if (!libutils::isEnabled(a)) return false;
	libutils::loadedAppIDList.push(a);
	if (libutils::hCQThreadEvent) ReleaseSemaphore(libutils::hCQThreadEvent, 1, NULL);
	return true;
}

extern "C" int32_t __stdcall CheckLibA(const char* LibAppID, const char* LibVer) {
	auto rlib = libutils::libFind(libutils::libList.begin(), libutils::libList.end(), LibAppID);
	if (rlib == libutils::libList.end()) return false;
	if (!LibVer) return true;
	return libutils::versionMatch(rlib->j["AppVer"].get<std::string>(), LibVer);
}

extern "C" int32_t __stdcall VersionMatch(const char* version, const char* range) {
	return libutils::versionMatch(version, range);
}

extern "C" bool __stdcall IsEnabled(const char* AppID) {
	auto rlib = libutils::libFind(libutils::libList.begin(), libutils::libList.end(), AppID);
	return rlib != libutils::libList.end() && rlib->appLoaded;
}

extern "C" bool __stdcall GetLibVersion(const char* AppID, char* VersionBack, size_t BufferSize) {
	auto rlib = libutils::libFind(libutils::libList.begin(), libutils::libList.end(), AppID);
	if (rlib == libutils::libList.end()) return false;
	return StringCchCopyA(VersionBack, BufferSize, rlib->j["AppVer"].get<std::string>().c_str()) == S_OK;
}
