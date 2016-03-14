/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//Define Graphics API to use Dx/Opengl
#define XCGRAPHICS_DX12
//#define XCGRAPHICS_DX11
//#define XCGRAPHICS_GL

#define _CRT_SECURE_NO_WARNINGS 1
#define NOMINMAX

#include <assert.h>
#define XCASSERT(x) assert(x)

#include <WinSock2.h>
#include <WS2tcpip.h>

#if defined(XCGRAPHICS_DX12)
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3d11_2.h>
#include <pix.h>
#include <D3Dcompiler.h>
#elif defined(XCGRAPHICS_DX11)
#include <d3d11.h>
#include <D3Dcompiler.h>
#elif defined(XCGRAPHICS_GL)
#include <GL/glew.h>
#include <GL/glut.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include <memory>
#include <windowsx.h>

#include "Engine/Memory/MemorySystemWin32.h"
#include "Engine/ErrorHandling/ErrorHandler.h"
#include "Engine/Graphics/GraphicsSpecificDefines.h"
#include "Engine/Utils/GraphicUtils.h"

typedef HANDLE ThreadHandle;
typedef HANDLE EVENTHANDLE;