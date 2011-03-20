#pragma once

// Windows Header Files
//#define WIN32_LEAN_AND_MEAN
#if defined(_WIN32_WINNT)
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT           0x510

#include <windows.h>
#include <gdiplus.h>

#include <crtdbg.h>
#include <tchar.h>

#define __MMOVERSION
#include "../MQ2Plugin.h"

#ifndef __CORE__
#include "../MQ2Main/MQ2Main.h"
#else
#define MMOBUGS_LOADER
#endif

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

#include "../MMOBugs.h"
#include "resource.h"
