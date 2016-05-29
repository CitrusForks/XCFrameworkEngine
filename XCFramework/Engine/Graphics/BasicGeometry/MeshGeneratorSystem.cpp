/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "MeshGeneratorSystem.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Resource/ResourceManager.h"

MeshGeneratorSystem::MeshGeneratorSystem(void)
{
}

MeshGeneratorSystem::~MeshGeneratorSystem(void)
{
}

void MeshGeneratorSystem::Init()
{
}

void MeshGeneratorSystem::Load()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");

    //Build the basic cube and push it to resources
    //Set up vertices
    XCMesh* mesh = (XCMesh*) resMgr.CreateResource("XCMesh", "CubeMesh");
    {
        mesh->InitDynamic("CubeMesh", ShaderType_LightTexture, "Placeholder", XCVec3Unaligned(1.0f, 1.0f, 1.0f), XCVec3Unaligned(0.0f, 0.0f, 0.0f));

        MeshData* meshData = mesh->CreateAndGetSubMesh();
        {
            //Add vertices
            meshData->AddVertex(-1.0f, -1.0f, -1.0f);
            meshData->AddVertex(-1.0f, 1.0f, -1.0f);
            meshData->AddVertex(1.0f, 1.0f, -1.0f);
            meshData->AddVertex(1.0f, -1.0f, -1.0f);

            meshData->AddVertex(-1.0f, -1.0f, 1.0f);
            meshData->AddVertex(-1.0f, 1.0f, 1.0f);
            meshData->AddVertex(1.0f, 1.0f, 1.0f);
            meshData->AddVertex(1.0f, -1.0f, 1.0f);

            //Add indices
            meshData->AddFace(MeshData::Face(0, 1, 2));
            meshData->AddFace(MeshData::Face(4, 6, 5));
            meshData->AddFace(MeshData::Face(0, 2, 3));
            meshData->AddFace(MeshData::Face(4, 7, 6));
            meshData->AddFace(MeshData::Face(4, 5, 1));
            meshData->AddFace(MeshData::Face(4, 1, 0));
            meshData->AddFace(MeshData::Face(3, 2, 6));
            meshData->AddFace(MeshData::Face(1, 5, 6));
            meshData->AddFace(MeshData::Face(3, 6, 7));
            meshData->AddFace(MeshData::Face(1, 6, 2));
            meshData->AddFace(MeshData::Face(4, 0, 3));
            meshData->AddFace(MeshData::Face(4, 3, 7));

            //Add UV's
            meshData->AddMapCoord(0.0f, 1.0f);
            meshData->AddMapCoord(0.0f, 0.0f);
            meshData->AddMapCoord(1.0f, 0.0f);
            meshData->AddMapCoord(1.0f, 1.0f);

            meshData->AddMapCoord(0.0f, 1.0f);
            meshData->AddMapCoord(0.0f, 0.0f);
            meshData->AddMapCoord(1.0f, 0.0f);
            meshData->AddMapCoord(1.0f, 1.0f);
        }
        //mesh->LoadDynamic();
    }

    ResourceHandle resHandle = {};
    resHandle.m_Resource = mesh;
    resHandle.m_serializerBuffer = nullptr;

    resMgr.AddResource(resHandle);
}

void MeshGeneratorSystem::Destroy()
{
}