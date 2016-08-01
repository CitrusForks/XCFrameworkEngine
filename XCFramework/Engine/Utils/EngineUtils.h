/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

const f32 Infinity = FLT_MAX;

u32 getFilelength(FILE* fp);
HRESULT ReadRawDataFromFile(const char* filename, UINT8** data, u32& size);

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

void SetThreadName(DWORD dwThreadID, const char* threadName);