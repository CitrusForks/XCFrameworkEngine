/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Engine/Resource/ResourceFactory.h"
#include "Engine/Graphics/XC_Textures/Texture2D.h"
#include "Engine/Graphics/XC_Textures/CubeTexture3D.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Engine/Graphics/XC_Mesh/XCMeshAssimp.h"
#include "Engine/Graphics/XC_Mesh/XCMeshFBX.h"
#include "Engine/Graphics/XC_Mesh/VectorFontMesh.h"

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
    if (resource->getResourceType() == RESOURCETYPE_TEXTURE2D || resource->getResourceType() == RESOURCETYPE_CUBETEXTURE3D)
    {
        resource->Load(rscPath);
    }
    else if (resource->getResourceType() == RESOURCETYPE_MESH)
    {
        float scaling = 0.0f;
        fscanf(packageFile, "%f", &scaling);
        ((XCMesh*)resource)->Load(rscPath, scaling);
    }
}

void ResourceFactory::loadResource(const void* buffer, IResource* const resource)
{
    resource->Load(buffer);
}