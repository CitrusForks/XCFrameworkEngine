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

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include <memory>
#include <windowsx.h>

//Engine Memory includes. To remove this
#include "Engine/Memory/MemorySystemWin32.h"

//Base Includes
#include "Base/Serializer/IBase.h"
#include "Base/BaseTypes.h"
#include "Base/XCMath/XCMathIncludes.h"
#include "Base/ErrorHandling/ErrorHandler.h"
#include "ErrorHandling/ErrorHandler.h"