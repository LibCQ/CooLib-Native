#pragma once

class cLibInfo
{
public:
	HMODULE hlib;
	std::string path;
	std::string name;
	nlohmann::json j;
	int32_t priority;
	bool loaded;
	cLibInfo(HMODULE _hlib, std::string _path, nlohmann::json _j) {
		hlib = _hlib;
		path = _path;
		j = _j;
		name = j["AppID"].get<std::string>();
		priority = j["priority"].get<int32_t>();
		loaded = false;
	}
};

extern std::vector<cLibInfo> libList; // ²å¼þ±í

extern void loadLib();
extern void unloadLib();
extern void reloadLib();

bool versionMatch(std::string rver, std::string ver);
bool upsort(cLibInfo i, cLibInfo j);
bool downsort(cLibInfo i, cLibInfo j);
std::vector<cLibInfo>::iterator libFind(std::vector<cLibInfo>::iterator _First, const std::vector<cLibInfo>::iterator _Last, const std::string& _Val);
