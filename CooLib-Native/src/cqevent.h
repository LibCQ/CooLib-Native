#pragma once

extern "C"
{
	const char* AppInfo();
	int32_t Initialize(int32_t AuthCode);
	int32_t _eventStartup();
	int32_t _eventExit();
}

