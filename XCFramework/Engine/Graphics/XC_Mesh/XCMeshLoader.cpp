/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "XCMeshLoader.h"

bool XCMeshLoader::LoadMesh(std::string meshFilePath, XCMesh& mesh)
{
    
    return true;
}

bool XCMeshLoader::Load3DS(std::string meshFilePath, XCMesh& mesh)
{
    return false;
}

bool XCMeshLoader::LoadFBX(std::string meshFilePath, XCMesh& mesh)
{
    return false;
}

bool XCMeshLoader::LoadUsingAssimp(std::string meshFilePath, XCMesh& mesh)
{
    return false;
}
