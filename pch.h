#pragma once

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include <crtdbg.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <fstream>

#ifndef ASSERT
#define ASSERT _ASSERT
#endif

#ifndef TRACE
namespace mq {
	extern "C" __declspec(dllimport) void DebugSpew(const char* szFormat, ...);
}
#define TRACE mq::DebugSpew
#endif

#include "LCDUI/LCDUI.h"
