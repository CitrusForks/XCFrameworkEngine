/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "LoadPackageFileFBTask.h"

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
    fbSystem.ParseAndLoadFile(RESOURCE_SCHEMA_FILEPATH, m_fbBuffer);
    fbSystem.ParseAndLoadFile(m_packageData, m_fbBuffer);

    auto FBResourcesRoot = GetFBResources(m_fbBuffer.GetBufferFromLoadedData());
    
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBTexture2D>>*, FBTexture2D>(FBResourcesRoot->FBTexture2DS(), resMgr, "Texture2D");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBXCMesh>>*, FBXCMesh>(FBResourcesRoot->FBXCMeshes(), resMgr, "XCMesh");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBXCMesh>>*, FBXCMesh>(FBResourcesRoot->FBMeshCustoms(), resMgr, "XCMeshCustom");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBXCMesh>>*, FBXCMesh>(FBResourcesRoot->FBMeshFBX(), resMgr, "XCMeshFBX");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBXCMesh>>*, FBXCMesh>(FBResourcesRoot->FBVectorFontMesh(), resMgr, "VectorFontMesh");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBCubeTexture3D>>*, FBCubeTexture3D>(FBResourcesRoot->FBCubeTexture3DS(), resMgr, "CubeTexture3D");

    Destroy();
}

void LoadPackageFileFBTask::Destroy()
{
    AsyncTask::Destroy();
}


