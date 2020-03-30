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

extern "C" const char* __stdcall LibInfo() { // 在这里填写应用相关信息
	return (const char*)R"(
// 一个示例 json，填入 LibInfo 的返回值中
{
  "ver": 1, // 版本号，填1
  "AppID": "com.example.demo", // AppID，与应用 AppID 保持一致
  "AppVer": "1.0.0", // 应用版本，应当符合 semver 版本号规定（https://semver.org/lang/zh-CN/）
  "require": { // 依赖库表
    "cn.coorg.coolib.testlib": "*" // 依赖库及需求版本（版本范围语法：https://docs.npmjs.com/misc/semver#ranges https://docs.npmjs.com/misc/semver#advanced-range-syntax）
  },
  "using": {
    "ExampleDecFunc": "cn.coorg.coolib.testlib::ExampleDecFunc"
  },
  "LibAPI": { // 接口表
    "ExampleAddFunc": "ExampleAddFunc", // API名 : DLL导出名
    "Dec": { // 命名空间，可嵌套
      "ExampleDecFunc": "ExampleDecFunc"
    }
  }
}
)";
}

// 将酷Q事件的对应函数搬到下方的 CooLib 事件中
// Startup -> LibCallback
// Enable -> AppCallback
// Disable -> DisableCallback
// Exit -> ExitCallback
// 并在酷Q相关消息事件中加入是否载入的判断（重要，由于 CooLib 是异步载入，酷Q可能会在以下事件调用前通知消息事件）

extern "C" int32_t __stdcall LibCallback(bool success, const char* callbackJson) { // 库载入事件
	if (success) {
		// 返回 json 参照 callback.sample.json
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
	return 0;
}

extern "C" int32_t __stdcall ExampleAddFunc(int32_t a, int32_t b) { // 一个示例函数
	return a + b;
}
