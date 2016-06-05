/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XC_ORBIS)
#define FAILED(x) x != SCE_OK
#endif

#include <assert.h>
#define XCASSERT(x) assert(x)

static void Logger(const char* format, ...)
{
    char messageBuffer[1024];

    va_list list;

    va_start(list, format);

    ::vsprintf_s(messageBuffer, format, list);
    strcat_s(messageBuffer, "\n");

#if defined(XC_ORBIS)
    printf(messageBuffer);
#else
    OutputDebugString(messageBuffer);
#endif

    va_end(list);
}

inline HRESULT ValidateResult(HRESULT hr)
{
    if (FAILED(hr))
    {
        __debugbreak();
        Logger("[FAILED] In file %s at %d with error code : %x", __FILE__, __LINE__, hr);
    }

    return hr;
}