/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(WIN_32)
#include "ApplicationFramework/Win32/pch_win32.h"
#elif defined(DURANGO)
#include "ApplicationFramework/Durango/pch_durango.h"
#elif defined(XC_ORBIS)
#include "ApplicationFramework/Orbis/pch_orbis.h"
#endif

#include "Engine/Utils/EngineUtils.h"
#include "Engine/System/SystemLocator.h"