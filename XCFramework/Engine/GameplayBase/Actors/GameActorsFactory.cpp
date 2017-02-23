/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "GameActorsFactory.h"

GameActorsFactory::GameActorsFactory()
{
}

GameActorsFactory::~GameActorsFactory()
{
}

void GameActorsFactory::InitFactory()
{
    m_gameActorsFactoryLock.Create();

    RegisterActors();
}

void GameActorsFactory::RegisterActors()
{
    RegisterObject<IActor>("IActor");
    RegisterObject<SceneRoot>("SceneRoot");
    RegisterObject<SimpleActor>("SimpleActor");
    RegisterObject<SimpleMeshActor>("SimpleMeshActor");
    RegisterObject<PhysicsActor>("Physics");
    RegisterObject<FontActor>("Font");
    RegisterObject<AnimatedActor>("AnimatedActor");
    RegisterObject<Door>("Door");
    RegisterObject<Waves>("Waves");
    RegisterObject<SimpleTerrain>("SimpleTerrain");
    RegisterObject<Terrain>("MultiTexturedTerrain");
    RegisterObject<Soldier>("Soldier");
    RegisterObject<PCSoldier>("PCSoldier");
    RegisterObject<NPCSoldier>("NPCSoldier");
    RegisterObject<Gun>("Gun");
    RegisterObject<Car>("Car");
    RegisterObject<PCCar>("PCCar");
    RegisterObject<NPCCar>("NPCCar");
    RegisterObject<Bullet>("Bullet");
    RegisterObject<SimpleSkyBox>("SimpleSkyBox");
    RegisterObject<TexturedPlane>("TexturedPlane");
    RegisterObject<LiveDriveTexturePlane>("LiveDriveTexturePlane");
}

IActor* GameActorsFactory::CreateActor(std::string actorName)
{
    BaseIDGenerator& baseIdGen = SystemLocator::GetInstance()->RequestSystem<BaseIDGenerator>("BaseIDGenerator");
    u32 baseId = baseIdGen.GetNextBaseObjectId();

    m_gameActorsFactoryLock.Enter();

    IActor* actor = (IActor*) CreateObject(actorName);
    actor->SetBaseObjectId(baseId);

    Logger("[GAME ACTORS FACTORY] Actor : %s  ID : %d", actorName.c_str(), baseId);

    m_gameActorsFactoryLock.Exit();

    return actor;
}

void GameActorsFactory::DestroyFactory()
{
    m_registeredActors.clear();

    m_gameActorsFactoryLock.Release();
}

void GameActorsFactory::GetAllActorTypes(GameActorsInfo* actorTypes)
{
    i32 index = 0;
    for (auto& actor : m_registeredActors)
    {
        strcpy_s(actorTypes[index].m_actorName, actor.second.c_str());
        index++;
    }
}

#if defined(EDITOR)

extern "C" __declspec(dllexport) void GetAllActorTypes(GameActorsInfo* info)
{
    GAMEACTORS_FACTORY->GetAllActorTypes(info);
}

extern "C" __declspec(dllexport) i32 GetNoOfActorTypes()
{
    return GAMEACTORS_FACTORY->GetNoOfActorTypes();
}

#endif