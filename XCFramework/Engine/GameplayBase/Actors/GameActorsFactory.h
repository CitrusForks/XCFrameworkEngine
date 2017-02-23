/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/ObjectFactory.h"
#include "Base/System/ISystem.h"
#include "Base/Serializer/BaseIDGenerator.h"

#include "Gameplay/GameActors/GameActorDefines.h"

struct GameActorsInfo
{
    char    m_actorName[50];
};

class GameActorsFactory : ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(GameActorsFactory)

    GameActorsFactory();
    virtual ~GameActorsFactory();

    void                            InitFactory();
    void                            DestroyFactory();

    void                            RegisterActors();

    IActor*                         CreateActor(std::string userFriendlyName);
    
    template<class T>
    T*                              CreateActor(std::string actorName);

    i32                             GetNoOfActorTypes() { return GAMEACTOR_MAX; }
    void                            GetAllActorTypes(GameActorsInfo* actorTypes);

private:
    std::map<EGameActorType, std::string>       m_registeredActors;
    CriticalSection                             m_gameActorsFactoryLock;
};

template<class T>
T* GameActorsFactory::CreateActor(std::string actorName)
{
    BaseIDGenerator& baseIdGen = SystemLocator::GetInstance()->RequestSystem<BaseIDGenerator>("BaseIDGenerator");
    u32 baseId = baseIdGen.GetNextBaseObjectId();
    
    m_gameActorsFactoryLock.Enter();

    IActor* actor = (IActor*)CreateObject(actorName);
    actor->Init(baseId);
    
    m_gameActorsFactoryLock.Exit();

    Logger("[GAME ACTORS FACTORY] Actor : %s  ID : %d", actorName.c_str(), baseId);

    return (T*)actor;
}