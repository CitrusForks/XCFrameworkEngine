/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//std, stl
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <queue>

//Thread
#include <future>
#include <atomic>
#include <mutex>

#include "Engine/XCMath/XCMathIncludes.h"

#if defined(WIN_32)
#include "Engine/ApplicationFramework/Win32/pch_win32.h"
#elif defined(DURANGO)
#include "Engine/ApplicationFramework/Durango/pch_durango.h"
#elif defined(XC_ORBIS)
#include "Engine/ApplicationFramework/Orbis/pch_orbis.h"
#endif

//Engine specific
#include "Engine/ErrorHandling/ErrorHandler.h"
#include "Engine/System/SystemLocator.h"

//Load stress
#define LOAD_STRESS_ENABLE 0