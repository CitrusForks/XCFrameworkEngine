/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"

//Flat buffers
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"

class FlatBuffersSystem : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(FlatBuffersSystem)

    FlatBuffersSystem();
    virtual ~FlatBuffersSystem();

    virtual void            Init();
    virtual void            Destroy();

    bool                    ParseAndLoadFile(std::string fileName);
    bool                    ParseAndLoadMeshFile(std::string fileName);
    const void*             GetBufferFromLoadedData() const { return m_fbParser.builder_.GetBufferPointer(); }

private:
    flatbuffers::Parser           m_fbParser;
    flatbuffers::Parser           m_fbParserMesh;
    static const char*            m_includeDirs[];
};