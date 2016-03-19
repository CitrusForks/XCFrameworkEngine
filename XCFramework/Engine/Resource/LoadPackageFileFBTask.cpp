/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "LoadPackageFileFBTask.h"

#include "ResourceManager.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"

LoadPackageFileFBTask::LoadPackageFileFBTask(std::string packageData)
    : m_packageData(packageData)
{
}

LoadPackageFileFBTask::~LoadPackageFileFBTask()
{
}

void LoadPackageFileFBTask::Init()
{
    AsyncTask::Init();
}

void LoadPackageFileFBTask::Run()
{
    AsyncTask::Run();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");

    //Read the file and create resources and load contents based on it
    //Load data
    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(RESOURCE_SCHEMA_FILEPATH);
    fbSystem.ParseAndLoadFile(m_packageData);

    auto FBResourcesRoot = GetFBResources(fbSystem.GetBufferFromLoadedData());
    
    //Read all the Texture2Ds
    IResource* resource = nullptr;
    
    for (auto it = FBResourcesRoot->FBTexture2DS()->begin(); it != FBResourcesRoot->FBTexture2DS()->end(); ++it)
    {
        resource = resMgr.GetResourceFactory().createResource("Texture2D");

        if (resource)
        {
            resource->Load((void*) *it);
            resMgr.AddResource(resource);
        }
    }

    //Read all the XCMeshS
    for (auto it = FBResourcesRoot->FBXCMeshes()->begin(); it != FBResourcesRoot->FBXCMeshes()->end(); ++it)
    {
        resource = resMgr.GetResourceFactory().createResource("XCMesh");

        if (resource)
        {
            resource->Load((void*) *it);
            resMgr.AddResource(std::move(resource));
        }
    }

    //Read all the XCMeshS
    for (auto it = FBResourcesRoot->FBMeshCustoms()->begin(); it != FBResourcesRoot->FBMeshCustoms()->end(); ++it)
    {
        resource = resMgr.GetResourceFactory().createResource("XCMeshCustom");

        if (resource)
        {
            resource->Load((void*)*it);
            resMgr.AddResource(std::move(resource));
        }
    }

    //Read all the XCMeshS
    if (FBResourcesRoot->FBMeshFBX())
    {
        for (auto it = FBResourcesRoot->FBMeshFBX()->begin(); it != FBResourcesRoot->FBMeshFBX()->end(); ++it)
        {
            resource = resMgr.GetResourceFactory().createResource("XCMeshFBX");

            if (resource)
            {
                resource->Load((void*)*it);
                resMgr.AddResource(std::move(resource));
            }
        }
    }

    //Read all the Font resources
    for (auto it = FBResourcesRoot->FBVectorFontMesh()->begin(); it != FBResourcesRoot->FBVectorFontMesh()->end(); ++it)
    {
        resource = resMgr.GetResourceFactory().createResource("VectorFontMesh");

        if (resource)
        {
            resource->Load((void*)*it);
            resMgr.AddResource(std::move(resource));
        }
    }

    //Read all the Texture3Ds
    for (auto it = FBResourcesRoot->FBCubeTexture3DS()->begin(); it != FBResourcesRoot->FBCubeTexture3DS()->end(); ++it)
    {
        resource = resMgr.GetResourceFactory().createResource("CubeTexture3D");

        if (resource)
        {
            resource->Load((void*)*it);
            resMgr.AddResource(std::move(resource));
        }
    }

    Destroy();
}

void LoadPackageFileFBTask::Destroy()
{
    AsyncTask::Destroy();
}