/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

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
#include "Graphics/Utils/GraphicUtils.h"