/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "WorldSceneLoader.h"
#include "World.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Assets/Packages/PackageConsts.h"

WorldSceneLoader::WorldSceneLoader(World& world, std::string packageData)
    : m_packageName(packageData)
    , m_worldRef(world)
{
}

WorldSceneLoader::~WorldSceneLoader()
{
}

void WorldSceneLoader::Init()
{
    AsyncTask::Init();

    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");
    m_actorFactory = &actorFactory;
}

void WorldSceneLoader::Run()
{
    AsyncTask::Run();

    FlatBuffersSystem& fbSystem = SystemLocator::GetInstance()->RequestSystem<FlatBuffersSystem>("FlatBuffersSystem");
    fbSystem.ParseAndLoadFile(OPENWORLD_SCHEMA_FILEPATH);
    fbSystem.ParseAndLoadFile(m_packageName);

    auto fbWorld = GetFBOpenWorldScene(fbSystem.GetBufferFromLoadedData());

    //Fonts
    if (fbWorld->FBFonts())
    {
        for (auto it = fbWorld->FBFonts()->begin(); it != fbWorld->FBFonts()->end(); ++it)
        {
            CreateLoadActor("FontActor", *it);
        }
    }

    //Soldiers
    for (auto it = fbWorld->FBSoldiers()->begin(); it != fbWorld->FBSoldiers()->end(); ++it)
    {
        std::string objectType = it->SoldierObjectType()->c_str();
        
        if (strcmp(objectType.c_str(), "PCSoldier") == 0)
        {
            CreateLoadActor("PCSoldier", *it);
        }
        else if (strcmp(objectType.c_str(), "NPCSoldier") == 0)
        {
            CreateLoadActor("NPCSoldier", *it);
        }
    }

    //Cars
    for (auto it = fbWorld->FBCars()->begin(); it != fbWorld->FBCars()->end(); ++it)
    {
        std::string objectType = it->CarObjectType()->c_str();

        if (strcmp(objectType.c_str(), "PCCar") == 0)
        {
            CreateLoadActor("PCCar", *it);
        }
        else if (strcmp(objectType.c_str(), "NPCCar") == 0)
        {
            CreateLoadActor("NPCCar", *it);
        }
    }

    //TexturedPlane
    if(fbWorld->FBTexturePlanes())
        for (auto it = fbWorld->FBTexturePlanes()->begin(); it != fbWorld->FBTexturePlanes()->end(); ++it)
        {
            CreateLoadActor("TexturedPlane", *it);
        }

#if defined(XCGRAPHICS_DX11)
    if (fbWorld->FBLiveDriveTexturePlanes())
        for (auto it = fbWorld->FBLiveDriveTexturePlanes()->begin(); it != fbWorld->FBLiveDriveTexturePlanes()->end(); ++it)
        {
            CreateLoadActor("LiveDriveTexturePlane", *it);
        }
#endif

    //MultiTextured Terrian
    for (auto it = fbWorld->FBMultiTexturedTerrains()->begin(); it != fbWorld->FBMultiTexturedTerrains()->end(); ++it)
    {
        CreateLoadActor("Terrain", *it);
    }

    //Waves
    for (auto it = fbWorld->FBWavess()->begin(); it != fbWorld->FBWavess()->end(); ++it)
    {
        //CreateLoadActor("Waves", *it);
    }

    //Skybox
    for (auto it = fbWorld->FBSimpleSkyBoxes()->begin(); it != fbWorld->FBSimpleSkyBoxes()->end(); ++it)
    {
        CreateLoadActor("SimpleSkyBox", *it);
    }

    Destroy();
}

IActor* WorldSceneLoader::CreateLoadActor(std::string actorName, const void* fbBuffer)
{
    IActor* actor = m_actorFactory->CreateActor(actorName);
    if (actor)
    {
        actor->PreLoad(fbBuffer);

        //Push to world
        m_worldRef.RequestAddActor(actor);
    }
    return actor;
}