/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "Utils/EngineUtils.h"

const DWORD MS_VC_EXCEPTION = 0x406D1388;

const WCHAR* charToWideChar(const char* str)
{
    const size_t cSize = strlen(str) + 1;

    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], str, cSize);

    return wc.c_str();
}

u32 getFilelength(FILE* fp)
{
    unsigned originalPosition = ftell(fp);
    fseek(fp, 0, SEEK_END);
    u32 endPosition = ftell(fp);

    fseek(fp, originalPosition, SEEK_SET);
    return endPosition;
}

HRESULT ReadRawDataFromFile(const char* filename, UINT8** data, u32& size)
{
    FILE* fp = fopen(filename, "rb");

    XCASSERT(fp);

    if (fp != nullptr)
    {
        //Get length of data
        u32 length = getFilelength(fp);

        *data = XCNEW(UINT8)[length];
        i32 res = fread(*data, sizeof(byte), length, fp);
        XCASSERT(res > 0);

        size = res;
        fclose(fp);
    }

    return S_OK;
}

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)


void SetThreadName(DWORD dwThreadID, const char* threadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
}