/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "stdafx.h"

#include "Engine/XCMath/XCMathIncludes.h"

const float Infinity = FLT_MAX;

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.

#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

//Colors and Materials
namespace Colors
{
    static XCVec4 White(1.0f, 1.0f, 1.0f, 1.0f);
    static XCVec4 Black(0.0f, 0.0f, 0.0f, 1.0f);
    static XCVec4 Red(1.0f, 0.0f, 0.0f, 1.0f);
    static XCVec4 Green(0.0f, 1.0f, 0.0f, 1.0f);
    static XCVec4 Blue(0.0f, 0.0f, 1.0f, 1.0f);
    static XCVec4 Yellow(1.0f, 1.0f, 0.0f, 1.0f);
    static XCVec4 Cyan(0.0f, 1.0f, 1.0f, 1.0f);
    static XCVec4 Magenta(1.0f, 0.0f, 1.0f, 1.0f);
}

XCVecIntrinsic4 CreatePlaneFromPoints(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3);
XCVecIntrinsic4 GetNormalFromPoints(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3);

bool IsVectorEqual(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2);
float XMVec3CosineDot(XCVecIntrinsic4& v1, XCVecIntrinsic4 &v2);

unsigned int getFilelength(FILE* fp);
HRESULT ReadRawDataFromFile(const char* filename, UINT8** data, unsigned int& size);

#if !defined(XC_ORBIS)
const WCHAR* charToWideChar(const char* str);
#endif

template<class T>
void Swap(T& obj1, T& obj2)
{
    T temp = obj1;
    obj1 = obj2;
    obj2 = temp;
}