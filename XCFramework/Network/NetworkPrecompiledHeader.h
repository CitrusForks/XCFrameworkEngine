/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define NOMINMAX

#include <WinSock2.h>

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

#include "Engine/Memory/MemorySystemWin32.h"

#include "Base/Serializer/IBase.h"
#include "Base/ErrorHandling/ErrorHandler.h"
#include "Base/XCMath/XCMathIncludes.h"

#include "Engine/System/SystemLocator.h"
#include "Engine/Utils/EngineUtils.h"