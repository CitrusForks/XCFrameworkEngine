/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "SceneGraph.h"

#include "Physics/PhysicsPlayground.h"

#include "Engine/Event/EventBroadcaster.h"

#include "Actors/GameActorsFactory.h"
#include "Actors/IActor.h"
#include "Actors/PhysicsActor.h"
#include "Actors/PlayableCharacterActor.h"
#include "Actors/NonPlayableCharacterActor.h"

#include "WorldEventTypes.h"
#include "SceneNode.h"
#include "SceneRoot.h"

#include "Gameplay/GameActors/Environment/Terrain/Terrain.h"

#include "Graphics/XCGraphics.h"

SceneGraph::SceneGraph()
{
    m_addRemoveFlag.store(false, std::memory_order_release);

    m_currentMainPlayableCharacter = 0;
    m_sceneReady = false;
    m_sceneQuiting = false;
}

SceneGraph::~SceneGraph(void)
{
}

void SceneGraph::Init(TaskManager& taskMgr)
{
    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");
    IActor* rootActor = actorFactory.CreateActor("SceneRoot");
    m_sceneGraph = XCNEW(XCTree<IActor*>)(rootActor);

    m_taskManager = &taskMgr;
    
    //Initialize the PendingTasks.
    m_scenePendingTasks = XCNEW(SceneGraphPendingTasks)(*this);
    m_taskManager->RegisterTask(m_scenePendingTasks);

    //Listen to events
    EventBroadcaster& broadCaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadCaster.AddListener(this);
}


//WorldPendingTasks -------------------------------------
void SceneGraphPendingTasks::Init()
{
    AsyncTask::Init();
}

void SceneGraphPendingTasks::AddTask(IPendingTask* task)
{
    m_pendingTaskLock.Enter();
    m_pendingTaskList.push(task);
    m_pendingTaskLock.Exit();
}

void SceneGraphPendingTasks::Run()
{
    AsyncTask::Run();

    IPendingTask* task = nullptr;

    m_pendingTaskLock.Enter();
    //Dequeue the pending load task one at a time
    if(m_pendingTaskList.size() > 0)
    {
        task = m_pendingTaskList.front();
    }
    m_pendingTaskLock.Exit();

    if(task)
    {
        IActor::IActor::ActorReturnState result = IActor::IActor::ActorReturnState_Success;

        if (task->m_TaskType == PendingTaskType_Load)
        {
            XCASSERT(((PendingTaskLoad*)task)->m_actor->GetActorState() == IActor::ActorState_Loading);
            result = ((PendingTaskLoad*)task)->m_actor->Load();
        }
        else if (task->m_TaskType == PendingTaskType_Unload)
        {
            XCASSERT(((PendingTaskLoad*)task)->m_actor->GetActorState() == IActor::ActorState_Unloading);
            result = ((PendingTaskLoad*)task)->m_actor->Unload();
        }

        switch (result)
        {
        case IActor::IActor::ActorReturnState_Success:
        {
            m_pendingTaskLock.Enter();
            m_pendingTaskList.pop();
            m_pendingTaskLock.Exit();

            XCDELETE(task);
            break;
        }

        case IActor::IActor::ActorReturnState_Processing:
            //Nothing. Let it process again
            break;

        case IActor::IActor::ActorReturnState_Error:
            //This is bad. Something went wrong. Delete this task then?
            XCASSERT(false);
            break;
        }
    }
}

void SceneGraphPendingTasks::Destroy()
{
    while (m_pendingTaskList.size() > 0)
    {
        //TODO: What if the actor are preloaded and we have some destruction to be done before the removal of this tasks.
        XCDELETE(m_pendingTaskList.front());
        m_pendingTaskList.pop();
    }

    m_pendingTaskLock.Release();

    //Clear all pending tasks
    AsyncTask::Destroy();
}

void SceneGraph::Update(f32 dt)
{
    //Actor Life cycle----------------------------------------------------------
    for (auto& sceneNode : m_sceneGraph->GetRootNode()->GetNodesMutable())
    {
        IActor::IActor::ActorReturnState returnState = IActor::IActor::ActorReturnState_Success;

        switch (sceneNode->GetActorState())
        {
        case IActor::ActorState_Initialize:
            returnState = sceneNode->Init();
            
            if (returnState == IActor::IActor::ActorReturnState_Success)
            {
                sceneNode->SetActorState(IActor::ActorState_MetaDataLoad);
            }
            break;

        case IActor::ActorState_MetaDataLoad:
            returnState = sceneNode->LoadMetaData(sceneNode->GetMetaData());

            if (returnState == IActor::IActor::ActorReturnState_Success)
            {
                //Request a load here.
                sceneNode->SetActorState(IActor::ActorState_Loading);
                RequestLoadActor(sceneNode);
            }
            break;

        case IActor::ActorState_Loading:
            if (sceneNode->IsActorLoaded())
            {
                sceneNode->SetActorState(IActor::ActorState_Loaded);
                AddActor(sceneNode);
            }
            break;

        case IActor::ActorState_Loaded:
            returnState = sceneNode->OnLoaded();

            if (returnState == IActor::IActor::ActorReturnState_Success)
            {
                sceneNode->SetActorState(IActor::ActorState_Ready);
            }
            break;

        case IActor::ActorState_Ready:
            if (sceneNode->IsInvalidated())
            {
                sceneNode->SetActorState(IActor::ActorState_Unloading);
                RequestRemoveActor(sceneNode);
            }
            else
            {
                returnState = sceneNode->Update(dt);
                XCASSERT(returnState == IActor::IActor::ActorReturnState_Success);
            }
            break;

        case IActor::ActorState_Unloading:
            if (!sceneNode->IsActorLoaded())
            {
                sceneNode->SetActorState(IActor::ActorState_Unloaded);
            }
            break;

        case IActor::ActorState_Unloaded:
            returnState = sceneNode->OnUnloaded();

            if (returnState == IActor::IActor::ActorReturnState_Success)
            {
                sceneNode->SetActorState(IActor::ActorState_Destroy);
            }
            break;
        case IActor::ActorState_Destroy:
            returnState = sceneNode->Destroy();

            if (returnState == IActor::IActor::ActorReturnState_Success)
            {
                //Remove it from the scene graph
                m_sceneGraph->RemoveNode(sceneNode);
            }
            break;

        default:
            XCASSERT(false);
            break;
        }
    }
}

void SceneGraph::Draw(XCGraphics& graphicsSystem)
{
    //Nothing here. Drawing of actors done in RenderingPool.
}

void SceneGraph::OnEvent(IEvent* evt)
{
    Event_Scene* evtWorld = dynamic_cast<Event_Scene*>(evt);
    
    if (evtWorld)
    {
        if (evtWorld->GetEventType() == EventType_SceneReady)
        {
            SetSceneReady(true);
        }
        else if (evtWorld->GetEventType() == EventType_TerrainLoaded)
        {
            PhysicsPlayground& plygrd = SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");
            plygrd.SetGravity(true);
        }
    }
}

void SceneGraph::RequestLoadActor(IActor* actor)
{
    SceneGraphPendingTasks::IPendingTask* task = XCNEW(SceneGraphPendingTasks::PendingTaskLoad)(SceneGraphPendingTasks::PendingTaskType_Load, actor);
    m_scenePendingTasks->AddTask(task);
}

void SceneGraph::AddActor(IActor* actor)
{
    i32 actorId = actor->GetBaseObjectId();
    Logger("[SCENE] Adding actor id : %d", actorId);

    //Check if we have PlayableCharacterActor. For PlayableCharacterActor, the Camera can be attached.
    PlayableCharacterActor* pcActor = dynamic_cast<PlayableCharacterActor*>(actor);
    if (pcActor != nullptr)
    {
        m_PlayableCharacterActors.push_back(actorId);
        m_currentMainPlayableCharacter = actorId;
    }

    //Check if we have NonPlayableCharacterActor. For NonPlayableCharacterActor, the AIBrain can be attached.
    NonPlayableCharacterActor* npcActor = dynamic_cast<NonPlayableCharacterActor*>(actor);
    if (npcActor != nullptr)
    {
        m_NonPlayableCharacterActors.push_back(actorId);
    }

    //Register it for rendering
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddRenderableObject((IRenderableObject*)actor, actor->GetBaseObjectId());
}

void SceneGraph::RequestRemoveActor(IActor* actor)
{
    SceneGraphPendingTasks::IPendingTask* task = XCNEW(SceneGraphPendingTasks::PendingTaskUnload)(SceneGraphPendingTasks::PendingTaskType_Unload, actor);
    m_scenePendingTasks->AddTask(task);
}

void SceneGraph::RemoveActor(IActor* actor)
{
    i32 key = actor->GetBaseObjectId();
    //Remove from the filtering vectors
    RemoveKey(m_PlayableCharacterActors, key);
    RemoveKey(m_NonPlayableCharacterActors, key);

    Logger("[SCENE] Removing actor id : %d", actor->GetBaseObjectId());

    //Unregister from rendering pool 
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().RemoveRenderableObject((IRenderableObject*)actor, actor->GetBaseObjectId());
}

bool SceneGraph::IsActorReady(i32 actorId)
{
    for(auto& sceneNode : m_sceneGraph->GetRootNode()->GetNodes())
    {
        if(sceneNode->GetBaseObjectId() == actorId)
        {
            return sceneNode->IsActorLoaded();
        }
    }
    
    //Not ready yet. So just return false.
    return false;
}

void SceneGraph::RemoveKey(std::vector<i32>& list, i32 key)
{
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        if ((*it) == key)
        {
            list.erase(it);
            break;
        }
    }
}

void SceneGraph::Destroy()
{
    m_sceneReady = false;

    m_PlayableCharacterActors.clear();
    m_NonPlayableCharacterActors.clear();

    m_scenePendingTasks->Destroy();
    m_taskManager->UnregisterTask(m_scenePendingTasks->GetThreadId());
    XCDELETE(m_scenePendingTasks);

    //Unload all actors
    for (auto& sceneNode : m_sceneGraph->GetRootNode()->GetNodesMutable())
    {
        sceneNode->Unload();
        sceneNode->Destroy();
    }

    XCDELETE(m_sceneGraph);
}

IActor* SceneGraph::GetActor(i32 instanceID)
{
    for(auto& sceneNode : m_sceneGraph->GetRootNode()->GetNodes())
    {
        if(sceneNode->GetBaseObjectId() == instanceID)
        {
            return sceneNode;
        }
    }

    return nullptr;
}

void SceneGraph::SetMainPlayableCharacter(i32 instanceId)
{ 
    IActor* actor = GetActor(instanceId);
    if (actor != nullptr)
    {
        //First disable current main playable character.
        IActor* currentActor = GetActor(m_currentMainPlayableCharacter);
        PlayableCharacterActor* pcActor = dynamic_cast<PlayableCharacterActor*>(currentActor);
        if (pcActor != nullptr)
        {
            pcActor->setControl(false);
        }

        //Next set the new playable character
        m_currentMainPlayableCharacter = instanceId;
        pcActor = dynamic_cast<PlayableCharacterActor*>(actor);
        if (pcActor != nullptr)
        {
            pcActor->setControl(true);
        }
    }
}

i32 SceneGraph::GetNextPlayableActor(i32 instanceId)
{
    //First find the match
    for (u32 index = 0; index < m_PlayableCharacterActors.size(); index++)
    {
        if (m_PlayableCharacterActors[index] == instanceId)
        {
            //Found match
            if (index == m_PlayableCharacterActors.size() - 1)
            {
                //Return first instance id
                return m_PlayableCharacterActors[0];
            }
            else
            {
                return m_PlayableCharacterActors[index + 1];
            }
        }
    }

    //If nothing just return the formal parameter
    return instanceId;
}


#if defined(EDITOR)

extern "C" __declspec(dllexport) i32 GetNoOfActors()
{
    return WORLD->GetNumOfActors();
}

extern "C" __declspec(dllexport) void GetActorsList()
{

}

#endif