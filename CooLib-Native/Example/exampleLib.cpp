// 此文件为节选代码，并不能直接编译，仅供参考

using json = nlohmann::json;

HMODULE examplelib;

extern "C" const char* __stdcall LibInfo() { // 在这里填写应用相关信息（参照libinfo.sample.json），注意不要保留json注释
	return (const char*)R"(
{
    "ver": 1,
    "AppID": "com.example.demo",
    "AppVer": "1.0.0",
    "lib": [
        {
            "LibAppID": "cn.coorg.coolib",
            "LibVer": "*"
        }
    ],
    "LibAPI": [
        {
            "func": "ExampleAddFunc",
            "?": "@@YGHHH@Z"
        }
    ]
}
)";
}

extern "C" int32_t __stdcall LibCallback(bool success, const char* callbackJson) { // 库载入事件，在这里初始化所有API接口
	if (success) {
		json j = json::parse(callbackJson);
		examplelib = LoadLibraryA(callbackJson["libPath"]["cn.coorg.coolib"].get<std::string>().c_str());
		// TODO: PUT YOUR CODE HERE
	}
	return 0;
}

extern "C" int32_t __stdcall AppCallback() { // 应用载入事件，在这里初始化应用
	return 0;
}

extern "C" int32_t __stdcall DisableCallback() { // 应用卸载事件，在这里结束应用
	return 0;
}

extern "C" int32_t __stdcall ExitCallback(const char* callbackJson) { // 库卸载事件，在这里结束库
	// TODO: PUT YOUR CODE HERE
	FreeLibrary(examplelib);
	return 0;
}

extern "C" int32_t __stdcall ExampleAddFunc(int32_t a, int32_t b) { // 一个示例函数
	return a + b;
}
