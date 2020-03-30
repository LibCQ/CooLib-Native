#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
//#include <d2d1.h>
//#pragma comment(lib, "d2d1")

#include <cstdint>
#include <future>

#include <shlwapi.h>
//#pragma comment(lib,"shlwapi.lib")
#include <queue>
#include <regex>

#include "boost/algorithm/string/regex.hpp"
#include "nlohmann/json.hpp"
#include "boost/format.hpp"
