/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

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
    fbSystem.ParseAndLoadFile(OPENWORLD_SCHEMA_FILEPATH, m_fbBuffer);
    fbSystem.ParseAndLoadFile(m_packageName, m_fbBuffer);

    auto fbWorld = GetFBOpenWorldScene(m_fbBuffer.GetBufferFromLoadedData());

    //Fonts
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBFont>>*>(fbWorld->FBFonts(), "FontActor");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBSoldier>>*>(fbWorld->FBPCSoldiers(), "PCSoldier");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBSoldier>>*>(fbWorld->FBNPCSoldiers(), "NPCSoldier");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBCar>>*>(fbWorld->FBPCCars(), "PCCar");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBCar>>*>(fbWorld->FBNPCCars(), "NPCCar");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBTexturePlane>>*>(fbWorld->FBTexturePlanes(), "TexturedPlane");
#if defined(LIVE_DRIVE_ENABLED)
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBLiveDriveTexturePlane>>*>(fbWorld->FBLiveDriveTexturePlanes(), "LiveDriveTexturePlane");
#endif
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBMultiTexturedTerrain>>*>(fbWorld->FBMultiTexturedTerrains(), "Terrain");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBWaves>>*>(fbWorld->FBWavess(), "Waves");
    LoadBuffer<const flatbuffers::Vector<flatbuffers::Offset<FBSimpleSkyBox>>*>(fbWorld->FBSimpleSkyBoxes(), "SimpleSkyBox");

    Destroy();
}