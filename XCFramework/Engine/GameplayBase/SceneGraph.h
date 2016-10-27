/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Thread/CriticalSection.h"
#include "Base/DataStructures/Trees/XCTree.h"

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Event/IEventListener.h"

#include "Engine/Physics/ParticleContact.h"
#include "Engine/Physics/XPhysics.h"

class IActor;
class SceneGraph;
class SceneNode;
class XCGraphics;

class SceneGraphPendingTasks : public AsyncTask
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

    SceneGraphPendingTasks(SceneGraph& scene)
        : AsyncTask("WorldPendingTasks")
        , m_parentWorld(scene)
    {
        m_pendingTaskLock.Create();
        m_bufferedPendingTaskList.clear();
    }

    void                        Init();
    void                        Run();
    void                        Destroy();

    void                        AddTask(IPendingTask* task);
    void                        ProcessFromScene();
    bool                        HasBufferedPendingTasks() { return m_bufferedPendingTaskList.size() > 0; }

private:

    std::queue<IPendingTask*>   m_pendingTaskList;
    std::vector<IPendingTask*>  m_bufferedPendingTaskList;
    CriticalSection             m_pendingTaskLock;

    SceneGraph&                 m_parentWorld;
};


class SceneGraphCollisionTask : public AsyncTask
{
public:
    SceneGraphCollisionTask(SceneGraph& scene)
        : AsyncTask("WorldCollisionTask")
        , m_parentScene(scene)
    {}
    ~SceneGraphCollisionTask() {}

    void                        Init();
    void                        Run();
    void                        Destroy();

private:
    SceneGraph&                 m_parentScene;
};

class SceneGraph : public ISystem, public IEventListener
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(SceneGraph)

    SceneGraph();
    virtual ~SceneGraph(void);
    
    virtual void                                Init(TaskManager& taskMgr);
    virtual void                                Update(f32 dt);
    virtual void                                Draw(XCGraphics& graphicsSystem);
    virtual void                                Destroy();

    void                                        CheckAllCollisions();
    
    XCTree<IActor*>&                            GetSceneGraphTree() { return *m_sceneGraph; }
    IActor*                                     GetActor(i32 _instanceId);
    i32                                         GetNumOfActors()                            {  return m_GameObjects.size();  }
    
    void                                        SetMainPlayableCharacter(i32 instanceId);
    i32                                         GetMainPlayableCharacter()                  { return m_currentMainPlayableCharacter;                      }
    IActor*                                     GetMainPlayableActor()                      { return m_GameObjects[m_currentMainPlayableCharacter]; }
    i32                                         GetInstanceIdOfActor(IActor* actor);
    i32                                         GetNextPlayableActor(i32 instanceId);
    
    bool                                        IsWorldReady()                              { return m_sceneReady;  }
    void                                        SetSceneReady(bool value)                   { m_sceneReady = true;  }

    void                                        RequestLoadActor(IActor* actor);
    void                                        RequestRemoveActor(i32 key);

    void                                        AddActor(IActor* actor);
    void                                        RemoveActor(i32 key);

    bool                                        IsActorReady(i32 actorId);
    bool                                        IsSceneQuiting()                            { return m_sceneQuiting; }

    void                                        EnablePhysics(bool enable)                  { m_isPhysicsEnabled = enable; XPhysics::s_enableGravity = enable; }
    bool                                        IsPhysicsEnabled()                          { return m_isPhysicsEnabled; }

    void                                        OnEvent(IEvent* evt);

protected: 
    bool                                        CheckCollision(PhysicsActor* obj1, PhysicsActor* obj2);

private:
    void                                        RemoveKey(std::vector<i32>& list, i32 key);

    XCTree<IActor*>*                                 m_sceneGraph;
    std::map<i32, IActor*>                           m_GameObjects;

    std::vector<i32>                                 m_PhysicsActorIDs;
    std::vector<i32>                                 m_PlayableCharacterActors;
    std::vector<i32>                                 m_NonPlayableCharacterActors;
    
    i32                                              m_currentMainPlayableCharacter;
    bool                                             m_sceneReady;

    //Based on MemoryBarriers - takes ~20 cycles to flag compare to CriticalSection 
    //More info on Lock https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
    std::atomic<bool>                                m_addRemoveFlag;       

    SceneGraphPendingTasks*                          m_scenePendingTasks;
    SceneGraphCollisionTask*                         m_sceneCollisionTask;
    bool                                             m_sceneQuiting;
    bool                                             m_isPhysicsEnabled;

    ParticleContact                                  m_particleContact;
    TaskManager*                                     m_taskManager;
};