// 此文件为节选代码，并不能直接编译，仅供参考

using json = nlohmann::json;

int32_t AuthCode;
bool loaded;

typedef int32_t(__stdcall* tAdd)(int32_t a, int32_t b);
tAdd Add;

extern "C" int32_t __stdcall _eventStartup()
{
	HMODULE h;
	h = GetModuleHandleA("CQP.dll");
	CQ_addLog = reinterpret_cast<tCQ_addLog>(GetProcAddress(h, "CQ_addLog"));

	// 通知 CooLib， 请*一定*执行本操作，否则 Coolib 会卡在等待加载操作
	// 此操作应当在所有业务代码*后*执行，用于标记酷Q相关业务代码已执行完毕
	typedef bool(__stdcall* tLibLoaded)(const char* a);
	char dllPath[MAX_PATH];
	if (GetModuleFileNameA(_hModule, dllPath, MAX_PATH) == 0) return 0;
	PathRemoveFileSpecA(dllPath);
	char szFilePath[MAX_PATH];
	lstrcpyA(szFilePath, dllPath);
	lstrcatA(szFilePath, "\\cn.coorg.coolib.cpk");
	HMODULE hcl = LoadLibraryA(szFilePath);
	if (hcl == 0) {
		lstrcpyA(szFilePath, dllPath);
		lstrcatA(szFilePath, "\\cn.coorg.coolib.dll");
		hcl = LoadLibraryA(szFilePath);
		if (hcl == 0) return 0;
	}
	tLibLoaded LibLoaded;
	DWORD l;
	LibLoaded = reinterpret_cast<tLibLoaded>(GetProcAddress(hcl, "LibLoaded"));
	if (LibLoaded) {
		LibLoaded("com.example.demo");
	}
	// 请保持 Coolib 加载，*不要*调用 FreeLibrary

	return 0;
}

extern "C" const char* __stdcall LibInfo() { // 在这里填写应用相关信息（参照libinfo.sample.json），注意不要保留json注释
	return (const char*)R"(
{
    "ver": 1,
    "AppID": "com.example.demo",
    "AppVer": "1.0.0",
    "require": {
        "cn.coorg.coolib.testlib": "*"
    },
    "using": {
        "Add": "cn.coorg.coolib.testlib::Add"
    },
    "LibAPI": {
		"ExampleAddFunc": "ExampleAddFunc",
		"Dec": {
			"ExampleDecFunc": "ExampleDecFunc"
		}
	}
}
)";
}

extern "C" int32_t __stdcall LibCallback(bool success, const char* callbackJson) { // 库载入事件
	if (success) {
		json j = json::parse(callbackJson);
		Add = reinterpret_cast<tAdd>(j["FuncAddr"]["Add"].get<int32_t>());
		// TODO: PUT YOUR CODE HERE
	}
	return 0;
}

extern "C" int32_t __stdcall AppCallback() { // 应用载入事件
	// TODO: PUT YOUR CODE HERE
	return 0;
}

extern "C" int32_t __stdcall DisableCallback() { // 应用卸载事件
	// TODO: PUT YOUR CODE HERE
	return 0;
}

extern "C" int32_t __stdcall ExitCallback(const char* callbackJson) { // 库卸载事件
	// TODO: PUT YOUR CODE HERE
	FreeLibrary(examplelib);
	return 0;
}

extern "C" int32_t __stdcall ExampleAddFunc(int32_t a, int32_t b) { // 一个示例函数
	return a + b;
}
