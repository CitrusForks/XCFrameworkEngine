/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

 // Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
 // This makes D3D objects work well in the debugger watch window, but slows down 
 // performance slightly.

#define NOMINMAX

#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

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

 //Define Graphics API to use Dx/Opengl
#define XCGRAPHICS_DX12
 //#define XCGRAPHICS_DX11
 //#define XCGRAPHICS_GL

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

#include "Graphics/GraphicsSpecificDefines.h"

#include "Engine/Memory/MemorySystemWin32.h"

//Base Includes
#include "Base/Serializer/IBase.h"
#include "Base/BaseTypes.h"
#include "Base/XCMath/XCMathIncludes.h"
#include "Base/ErrorHandling/ErrorHandler.h"

//Engine Includes
#include "Engine/System/SystemLocator.h"
#include "Engine/Utils/EngineUtils.h"


#include "Graphics/Utils/GraphicUtils.h"