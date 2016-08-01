/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Thread/CriticalSection.h"
#include "Engine/Event/IEventListener.h"

#include "Gameplay/XCPhysics/ParticleContact.h"

class World;

class WorldPendingTasks : public AsyncTask
{
public:
    enum PendingTaskType
    {
        PENDINGTASK_ADD,
        PENDINGTASK_REMOVE,
    };

    class IPendingTask
    {
    public:
        IPendingTask(PendingTaskType type)
            : m_TaskType(type)
        {}

        PendingTaskType         m_TaskType;
    };

    class PendingTaskAdd : public IPendingTask
    {
    public:
        PendingTaskAdd(PendingTaskType type, IActor* actor)
            : IPendingTask(type)
            , m_actor(actor)
        {}

        IActor* m_actor;
    };

    class PendingTaskRemove : public IPendingTask
    {
    public:
        PendingTaskRemove(PendingTaskType type, i32 actorId)
            : IPendingTask(type)
            , m_actorId(actorId)
        {}

        i32                     m_actorId;
    };

    WorldPendingTasks(World& world)
        : AsyncTask("WorldCollisionTask")
        , m_parentWorld(world)
    {
        m_pendingTaskLock.Create();
        m_bufferedPendingTaskList.clear();
    }

    void                        Init();
    void                        Run();
    void                        Destroy();

    void                        AddTask(IPendingTask* task);
    void                        ProcessFromWorld();
    bool                        HasBufferedPendingTasks() { return m_bufferedPendingTaskList.size() > 0; }

private:

    std::queue<IPendingTask*>   m_pendingTaskList;
    std::vector<IPendingTask*>  m_bufferedPendingTaskList;
    CriticalSection             m_pendingTaskLock;

    World&                      m_parentWorld;
};


class WorldCollisionTask : public AsyncTask
{
public:
    WorldCollisionTask(World& world)
        : AsyncTask("WorldCollisionTask")
        , m_parentWorld(world)
    {}
    ~WorldCollisionTask() {}

    void                        Init();
    void                        Run();
    void                        Destroy();

private:
    World&                      m_parentWorld;
};

class World : public ISystem, public IEventListener
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(World)

    World();
    virtual ~World(void);
    
    virtual void                                Init(TaskManager& taskMgr);
    virtual void                                Update(f32 dt);
    virtual void                                Draw(XC_Graphics& graphicsSystem);
    virtual void                                Destroy();
    
    void                                        RequestAddActor(IActor* actor);
    void                                        CheckAllCollisions();
    bool                                        doesCollide(IActor* obj1, IActor* obj2);
    
    IActor*                                     GetActor(i32 _instanceId);
    i32                                         GetNumOfActors()                            {  return m_GameObjects.size();  }
    
    void                                        SetMainPlayableCharacter(i32 instanceId);
    i32                                         GetMainPlayableCharacter()                  { return m_currentMainPlayableCharacter;                      }
    IActor*                                     GetMainPlayableActor()                      { return m_GameObjects[m_currentMainPlayableCharacter]; }
    i32                                         GetInstanceIdOfActor(IActor* actor);
    i32                                         GetNextPlayableActor(i32 instanceId);
    
    bool                                        IsWorldReady()                              { return m_worldReady;  }
    void                                        SetWorldReady(bool value)                   { m_worldReady = true;  }

    void                                        RequestRemoveActor(i32 key);

    void	                                    AddActor(IActor* actor);
    void                                        RemoveActor(i32 key);

    bool                                        IsActorReady(i32 actorId);
    bool                                        IsWorldQuiting()                            { return m_worldQuiting; }

    void                                        EnablePhysics(bool enable)                  { m_isPhysicsEnabled = enable; XPhysics::s_enableGravity = enable; }
    bool                                        IsPhysicsEnabled()                          { return m_isPhysicsEnabled; }

    void                                        OnEvent(IEvent* evt);

protected: 
    bool                                        CheckCollision(PhysicsActor* obj1, PhysicsActor* obj2);

private:
    void                                        processRemovePendingList();
    void                                        removeKey(std::vector<i32>& list, i32 key);


    std::map<i32, IActor*>                           m_GameObjects;
    std::vector<i32>                                 m_PhysicsActorIDs;
    std::vector<i32>                                 m_PlayableCharacterActors;
    std::vector<i32>                                 m_NonPlayableCharacterActors;
    
    i32                                              m_currentMainPlayableCharacter;
    bool                                             m_worldReady;

    //Based on MemoryBarriers - takes ~20 cycles to flag compare to CriticalSection 
    //More info on Lock https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
    std::atomic<bool>                                m_addRemoveFlag;       

    WorldPendingTasks*                               m_worldPendingTasks;
    WorldCollisionTask*                              m_worldCollisionTask;
    bool                                             m_worldQuiting;
    bool                                             m_isPhysicsEnabled;

    ParticleContact                                  m_particleContact;
    TaskManager*                                     m_taskManager;
};