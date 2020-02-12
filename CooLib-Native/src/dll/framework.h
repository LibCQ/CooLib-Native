#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>

#include <cstdint>

#include <shlwapi.h>
//#pragma comment(lib,"shlwapi.lib")
#include <queue>

#include "boost/algorithm/string.hpp"
#include "nlohmann/json.hpp"
#include "boost/format.hpp"
