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
{
    m_resourceCount = 0;
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


IResource* ResourceFactory::createResource(std::string classKey, std::string userFriendlyName)
{
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->Init(++m_resourceCount, userFriendlyName);

    return outResource;
}

IResource* ResourceFactory::createResource(std::string classKey)
{
    //Use this for Flatbuffers
    IResource* outResource = (IResource*) CreateObject(classKey);
    outResource->Init(++m_resourceCount, "");

    return outResource;
}

void ResourceFactory::loadResource(FILE* packageFile, IResource* const resource)
{
    char rscPath[256];

    fscanf(packageFile, "%s", rscPath);
    //Based on every resources init parameters, load it.
    if (resource->GetResourceType() == RESOURCETYPE_TEXTURE2D || resource->GetResourceType() == RESOURCETYPE_CUBETEXTURE3D)
    {
        resource->Load(rscPath);
    }
    else if (resource->GetResourceType() == RESOURCETYPE_MESH)
    {
        f32 scaling = 0.0f;
        fscanf(packageFile, "%f", &scaling);
        ((XCMesh*)resource)->Load(rscPath, scaling);
    }
}

void ResourceFactory::loadResource(const void* buffer, IResource* const resource)
{
    resource->Load(buffer);
}