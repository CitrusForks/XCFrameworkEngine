/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/Resource/ResourceFactory.h"
#include "Graphics/XCTextures/Texture2D.h"
#include "Graphics/XCTextures/CubeTexture3D.h"
#include "Graphics/XCMesh/XCMesh.h"
#include "Graphics/XCMesh/XCMeshAssimp.h"
#include "Graphics/XCMesh/XCMeshFBX.h"
#include "Graphics/XCMesh/VectorFontMesh.h"

ResourceFactory::ResourceFactory()
    : m_resourceCount(0)
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
    m_resourceCount = 0;
}

IResource* ResourceFactory::CreateResource(std::string classKey, std::string userFriendlyName)
{
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->SetBaseObjectId(++m_resourceCount);
    outResource->Init(userFriendlyName);

    return outResource;
}

IResource* ResourceFactory::CreateResource(std::string classKey)
{
    //Use this for Flatbuffers
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->SetBaseObjectId(++m_resourceCount);

    char usrFrndName[1024];
    sprintf(usrFrndName, "%s+%d", classKey, m_resourceCount);
    outResource->Init(usrFrndName);

    return outResource;
}