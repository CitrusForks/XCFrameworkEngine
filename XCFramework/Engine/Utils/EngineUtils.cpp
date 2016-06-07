/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "Utils/EngineUtils.h"

const WCHAR* charToWideChar(const char* str)
{
    const size_t cSize = strlen(str) + 1;

    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], str, cSize);

    return wc.c_str();
}

unsigned int getFilelength(FILE* fp)
{
    unsigned originalPosition = ftell(fp);
    fseek(fp, 0, SEEK_END);
    unsigned int endPosition = ftell(fp);

    fseek(fp, originalPosition, SEEK_SET);
    return endPosition;
}

HRESULT ReadRawDataFromFile(const char* filename, UINT8** data, unsigned int& size)
{
    FILE* fp = fopen(filename, "rb");

    XCASSERT(fp);

    if (fp != nullptr)
    {
        //Get length of data
        unsigned int length = getFilelength(fp);

        *data = new UINT8[length];
        int res = fread(*data, sizeof(byte), length, fp);
        XCASSERT(res > 0);

        size = res;
        fclose(fp);
    }

    return S_OK;
}