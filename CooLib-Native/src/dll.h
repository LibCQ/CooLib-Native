#pragma once

#include "pch.h"

extern HMODULE _hModule;

class LibInfo
{
public:
	HMODULE hlib;
	std::string path;
	std::string name;
	nlohmann::json j;
	int32_t priority;
	bool loaded;
	LibInfo(HMODULE _hlib, std::string _path, nlohmann::json _j) {
		hlib = _hlib;
		path = _path;
		j = _j;
		name = j["AppID"].get<std::string>();
		priority = j["priority"].get<int32_t>();
		loaded = false;
	}
};

bool upsort(LibInfo i, LibInfo j);
bool downsort(LibInfo i, LibInfo j);
std::vector<LibInfo>::iterator libFind(std::vector<LibInfo>::iterator _First, const std::vector<LibInfo>::iterator _Last, const std::string& _Val);