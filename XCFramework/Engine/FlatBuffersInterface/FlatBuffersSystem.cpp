/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"

const char* FlatBuffersSystem::m_includeDirs[] = { RESOURCE_SCHEMA_FOLDERPATH, RESOURCE_DATA_FOLDERPATH, SHADER_SCHEMA_FILEPATH, nullptr };

FlatBuffersSystem::FlatBuffersSystem()
{
}

FlatBuffersSystem::~FlatBuffersSystem()
{
}

void FlatBuffersSystem::Init()
{
}

void FlatBuffersSystem::Destroy()
{
}

bool FlatBuffersSystem::ParseAndLoadFile(std::string fileName, FBBuffer& buffer)
{
    std::string rawData;
    
    bool result = flatbuffers::LoadFile(fileName.c_str(), false, &rawData);
    XCASSERT(result);

    result = buffer.m_fbParser.Parse(rawData.c_str(), m_includeDirs, nullptr);
    XCASSERT(result);

    return result;
}