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

#include "Physics/ParticleContact.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"

class IActor;
class SceneGraph;
class SceneNode;
class XCGraphics;

class SceneGraphPendingTasks : public AsyncTask
{
public:
    enum PendingTaskType
    {
        PendingTaskType_Load,
        PendingTaskType_Unload,
    };

    class IPendingTask
    {
    public:
        IPendingTask(PendingTaskType type)
            : m_TaskType(type)
        {}

        PendingTaskType         m_TaskType;
    };

    class PendingTaskLoad : public IPendingTask
    {
    public:
        PendingTaskLoad(PendingTaskType type, IActor* actor)
            : IPendingTask(type)
            , m_actor(actor)
        {}

        IActor* m_actor;
    };

    class PendingTaskUnload : public IPendingTask
    {
    public:
        PendingTaskUnload(PendingTaskType type, IActor* actor)
            : IPendingTask(type)
            , m_actorRef(actor)
        {}

        IActor*                 m_actorRef;
    };

    SceneGraphPendingTasks(SceneGraph& scene)
        : AsyncTask("WorldPendingTasks")
    {
        m_pendingTaskLock.Create();
    }

    void                        Init();
    void                        Run();
    void                        Destroy();

    void                        AddTask(IPendingTask* task);

private:

    std::queue<IPendingTask*>   m_pendingTaskList;
    CriticalSection             m_pendingTaskLock;
};


class SceneGraph : public ISystem, public IEventListener
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(SceneGraph)

    SceneGraph();
    virtual ~SceneGraph(void);
    
    virtual void                                     Init(TaskManager& taskMgr);
    virtual void                                     Update(f32 dt);
    virtual void                                     Draw(XCGraphics& graphicsSystem);
    virtual void                                     Destroy();

    XCTree<IActor*>&                                 GetSceneGraphTree() { return *m_sceneGraph; }
    IActor*                                          GetActor(i32 _instanceId);

    void                                             SetMainPlayableCharacter(i32 instanceId);
    i32                                              GetMainPlayableCharacter() { return m_currentMainPlayableCharacter; }
    IActor*                                          GetMainPlayableActor() { return GetActor(m_currentMainPlayableCharacter); }
    i32                                              GetNextPlayableActor(i32 instanceId);

    bool                                             IsWorldReady() { return m_sceneReady; }
    void                                             SetSceneReady(bool value) { m_sceneReady = true; }

    //Async removal of actors.
    void                                             RequestLoadActor(IActor* actor);
    void                                             RequestRemoveActor(IActor* actor);

    void                                             AddActor(IActor* actor);
    void                                             RemoveActor(IActor* actor);

    bool                                             IsActorReady(i32 actorId);
    bool                                             IsSceneQuiting() { return m_sceneQuiting; }

    void                                             OnEvent(IEvent* evt);
    FlatBuffersSystem::FBBuffer&                     GetMutableFlatBuffer() { return m_fbBuffer; }

private:
    void                                             RemoveKey(std::vector<i32>& list, i32 key);

    XCTree<IActor*>*                                 m_sceneGraph;
    std::vector<i32>                                 m_PlayableCharacterActors;
    std::vector<i32>                                 m_NonPlayableCharacterActors;
    
    i32                                              m_currentMainPlayableCharacter;
    bool                                             m_sceneReady;

    //Based on MemoryBarriers - takes ~20 cycles to flag compare to CriticalSection 
    //More info on Lock https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
    std::atomic<bool>                                m_addRemoveFlag;       

    SceneGraphPendingTasks*                          m_scenePendingTasks;
    bool                                             m_sceneQuiting;

    TaskManager*                                     m_taskManager;
    FlatBuffersSystem::FBBuffer                      m_fbBuffer;
};