/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "GameActorsFactory.h"

GameActorsFactory::GameActorsFactory()
    : m_actorsCount(0)
{
}

GameActorsFactory::~GameActorsFactory()
{
}

void GameActorsFactory::InitFactory()
{
    RegisterActors();
}

void GameActorsFactory::RegisterActors()
{
    RegisterObject<IActor>("IActor");
    RegisterObject<SimpleActor>("SimpleActor");
    RegisterObject<SimpleMeshActor>("SimpleMeshActor");
    RegisterObject<PhysicsActor>("Physics");
    RegisterObject<FontActor>("FontActor");
    RegisterObject<AnimatedActor>("AnimatedActor");
    RegisterObject<Door>("Door");
    RegisterObject<Waves>("Waves");
    RegisterObject<SimpleTerrain>("SimpleTerrain");
    RegisterObject<Terrain>("Terrain");
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
    std::unique_lock<std::mutex> m(m_gameActorsFactoryLock);

    IActor* actor = (IActor*) CreateObject(actorName);
    actor->Init(++m_actorsCount);

    Logger("[GAME ACTORS FACTORY] Actor : %s  ID : %d", actorName.c_str(), m_actorsCount);

    m.unlock();

    return actor;
}

void GameActorsFactory::LoadActor(FILE* packageFile, IActor* const resource)
{
    //char rscPath[256];
  
    //fscanf(packageFile, "%s", rscPath);
    //Based on every resources init parameters, load it.
    /*if (resource->getResourceType() == RESOURCETYPE_TEXTURE2D)
    {
        resource->load(rscPath);
    }
    else if (resource->getResourceType() == RESOURCETYPE_MESH)
    {
        float scaling = 0.0f;
        fscanf(packageFile, "%f", &scaling);
        ((XCMesh*)resource)->load(rscPath, scaling);
    }*/
}

void GameActorsFactory::DestroyFactory()
{
    m_registeredActors.clear();
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