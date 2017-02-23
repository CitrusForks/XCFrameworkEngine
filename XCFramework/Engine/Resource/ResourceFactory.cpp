/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Base/Serializer/BaseIDGenerator.h"

#include "Engine/Resource/ResourceFactory.h"

#include "Graphics/XCTextures/Texture2D.h"
#include "Graphics/XCTextures/CubeTexture3D.h"
#include "Graphics/XCMesh/XCMesh.h"
#include "Graphics/XCMesh/XCMeshAssimp.h"
#include "Graphics/XCMesh/XCMeshFBX.h"
#include "Graphics/XCMesh/VectorFontMesh.h"

ResourceFactory::ResourceFactory()
{
}

ResourceFactory::~ResourceFactory()
{
}

void ResourceFactory::InitFactory()
{
    RegisterObject<Texture2D>("Texture2D");
    RegisterObject<XCMesh>("XCMesh");
    RegisterObject<XCMeshAssimp>("XCMeshCustom");
    RegisterObject<XCMeshFBX>("XCMeshFBX");
    RegisterObject<VectorFontMesh>("VectorFontMesh");
    RegisterObject<CubeTexture3D>("CubeTexture3D");
}

void ResourceFactory::DestroyFactory()
{
}

IResource* ResourceFactory::CreateResource(std::string classKey, std::string userFriendlyName)
{
    BaseIDGenerator& baseIdGen = SystemLocator::GetInstance()->RequestSystem<BaseIDGenerator>("BaseIDGenerator");
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->SetBaseObjectId(baseIdGen.GetNextBaseObjectId());
    outResource->Init(userFriendlyName);

    return outResource;
}

IResource* ResourceFactory::CreateResource(std::string classKey)
{
    //Use this for Flatbuffers
    BaseIDGenerator& baseIdGen = SystemLocator::GetInstance()->RequestSystem<BaseIDGenerator>("BaseIDGenerator");
    u32 baseId = baseIdGen.GetNextBaseObjectId();
    
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->SetBaseObjectId(baseId);

    char usrFrndName[1024];
    sprintf(usrFrndName, "%s+%d", classKey, baseId);
    outResource->Init(usrFrndName);

    return outResource;
}