#pragma once

#include <mq/Plugin.h>

using std::min;
using std::max;

#include <windows.h>
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
#define TRACE DebugSpew
#endif


#include "LCDUI/LCDUI.h"
#include "LCDSDK/lglcd.h"

#include "resource.h"
