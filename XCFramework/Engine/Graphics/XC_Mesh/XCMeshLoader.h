/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class XCMesh;

#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Engine/Graphics/XC_Mesh/MeshData.h"

class XCMeshLoader : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XCMeshLoader)

    bool    LoadMesh(std::string meshFilePath, XCMesh& mesh);

protected:
    
    bool    Load3DS(std::string meshFilePath, XCMesh& mesh);
    bool    LoadFBX(std::string meshFilePath, XCMesh& mesh);
    bool    LoadUsingAssimp(std::string meshFilePath, XCMesh& mesh);
};