#pragma once
#include "..\dll\pch.h"

using json = nlohmann::json;

class cLibInfo
{
public:
	HMODULE hlib;
	std::string path;
	std::string name;
	json j;
	bool loaded;
	bool appLoaded;
	cLibInfo(HMODULE _hlib, std::string _path, std::string _j) {
		hlib = _hlib;
		path = _path;

		j = json::parse(_j);
		name = j["AppID"].get<std::string>();
		loaded = false;
	}
};

namespace libutils {
	extern std::vector<cLibInfo> libList;
	extern std::queue<std::string> loadedAppIDList;
	extern HANDLE hCQThread;
	extern HANDLE hCQThreadEvent;
	extern HANDLE hCQThreadExitEvent;

	extern DWORD WINAPI CQThreadProc(LPVOID lpParameter);
	extern void loadLib(std::vector<cLibInfo> tlibList);
	std::vector<cLibInfo> loadLib_1();
	void LibCallback(std::vector<cLibInfo>* tlibList);
	void appCallback(std::vector<cLibInfo>* tlibList);
	bool isEnabled(std::string AppID);
	extern void unloadLib();
	//extern void reloadLib();

	bool versionMatch(std::string rver, std::string ver);
	std::vector<cLibInfo> LibSort(const std::vector<cLibInfo> sortLib);
	std::vector<std::string> TSort(const std::map<std::string, int> _indegree, const std::map<std::string, std::vector<std::string>> adj);
	std::vector<cLibInfo>::iterator libFind(std::vector<cLibInfo>::iterator _First, const std::vector<cLibInfo>::iterator _Last, const std::string& _Val);
}
