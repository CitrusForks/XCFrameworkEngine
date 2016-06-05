/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once
#include "XCMesh.h"

//#define WRITE_DATA_TO_FILE 1

class XC3DSMeshLoader
{
public:
    XC3DSMeshLoader(void);
    ~XC3DSMeshLoader(void);

    bool loadMeshFromFile(std::string fileName, XCMesh* const outMesh);

protected:
    struct Chunk
    {
        unsigned short           chunkId;          //2 bytes
        unsigned int             chunkLength;        //4bytes
        unsigned char            c_char;
        unsigned short           amtOfData;
        unsigned short           valToNextChunk;
    };

    static const unsigned int           CHUNK_MAIN                  = 0x4d4d;
    static const unsigned int           CHUNK_3DEDITOR              = 0x3d3d;
    static const unsigned int           CHUNK_OBJECT                = 0x4000;
    static const unsigned int           CHUNK_TRIANGULARMESHLIST    = 0x4100;
    static const unsigned int           CHUNK_VERTICESLIST          = 0x4110;
    static const unsigned int           CHUNK_FACESLIST             = 0x4120;
    static const unsigned int           CHUNK_MAPCOORDLIST          = 0x4140;
};