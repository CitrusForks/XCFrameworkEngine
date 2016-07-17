/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define NOMINMAX

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

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

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <memory>
#include <windowsx.h>

#include "Base/XCTypes.h"

#include "Base/ErrorHandling/ErrorHandler.h"

//Engine Memory includes. To remove this
#include "Engine/Memory/MemorySystemWin32.h"

//Base Includes
#include "Base/Serializer/IBase.h"
#include "Base/XCMath/XCMathIncludes.h"
