// Bitmap2XCHeightmapExporter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wingdi.h>

int main(int argc, const char* argv[])
{

    FILE *pFilePtr;
    int error;

    unsigned int count;

    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;

    //Open the height map file in binary
    error = fopen_s(&pFilePtr, argv[0], "rb");
    if (error != 0)
    {
        return;
    }

    //Read the file header
    count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, pFilePtr);
    if (count != 1)
    {
        __debugbreak();
        return;
    }

    //Read in the bitmap info header
    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFilePtr);
    if (count != 1)
    {
        __debugbreak();
        return;
    }

    unsigned int rows = bitmapInfoHeader.biWidth;
    unsigned int cols = bitmapInfoHeader.biHeight;

    totalPixels = m_rows * m_cols;

    int imageSize = m_rows * m_cols * 3;

    m_pBitmapImage = new unsigned char[imageSize];

    //Move to first position within the bitmap file
    fseek(pFilePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //Read in the bitmap image data
    count = fread(m_pBitmapImage, 1, imageSize, pFilePtr);
    if (count != imageSize)
    {
        __debugbreak();
        return;
    }

    //Close the file
    fclose(pFilePtr);

	return 0;
}

