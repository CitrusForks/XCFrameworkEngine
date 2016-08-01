/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/World.h"
#include "Gameplay/XCPhysics/CollisionDetection.h"
#include "Gameplay/GameActors/PhysicsActor.h"
#include "Gameplay/GameActors/Environment/Terrain/Terrain.h"
#include "Gameplay/GameActors/PlayableCharacterActor.h"
#include "Gameplay/GameActors/NonPlayableCharacterActor.h"
#include "Gameplay/WorldEventTypes.h"

#include "Graphics/XC_Graphics.h"
#include "Engine/Event/EventBroadcaster.h"

World::World()
{
    m_addRemoveFlag.store(false, std::memory_order_release);

    m_currentMainPlayableCharacter = 0;
    m_worldReady = false;
    m_worldQuiting = false;

    EnablePhysics(true);
}


World::~World(void)
{
    XCDELETE(m_worldPendingTasks);
    XCDELETE(m_worldCollisionTask);
}

void World::Init(TaskManager& taskMgr)
{
    m_taskManager = &taskMgr;
    
    //Initialize the WorldPendingTasks.
    m_worldPendingTasks = XCNEW(WorldPendingTasks)(*this);
    m_taskManager->RegisterTask(m_worldPendingTasks);

    //Initialize the collision thread
    m_worldCollisionTask = XCNEW(WorldCollisionTask)(*this);
    m_worldCollisionTask->SetTaskPriority(THREAD_PRIORITY_BELOW_NORMAL);
    m_taskManager->RegisterTask(m_worldCollisionTask);

    //Listen to events
    EventBroadcaster& broadCaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadCaster.AddListener(this);
}

//WorldCollisionTask -------------------------------------
void WorldCollisionTask::Init()
{
    AsyncTask::Init();
}

void WorldCollisionTask::Run()
{
    AsyncTask::Run();

    if (m_parentWorld.IsPhysicsEnabled())
    {
        if (m_parentWorld.IsWorldReady())
        {
            m_parentWorld.CheckAllCollisions();
        }
    }
}

void WorldCollisionTask::Destroy()
{
    AsyncTask::Destroy();
}


//WorldPendingTasks -------------------------------------
void WorldPendingTasks::Init()
{
    AsyncTask::Init();
    m_bufferedPendingTaskList.clear();
}

void WorldPendingTasks::AddTask(IPendingTask* task)
{
    m_pendingTaskLock.Enter();
    m_pendingTaskList.push(task);
    m_pendingTaskLock.Exit();
}

void WorldPendingTasks::Run()
{
    AsyncTask::Run();

    IPendingTask* task = nullptr;

    m_pendingTaskLock.Enter();
    //Dequeue the pending load task one at a time
    if(m_pendingTaskList.size() > 0)
    {
        task = m_pendingTaskList.front();
        m_pendingTaskList.pop();
    }
    m_pendingTaskLock.Exit();

    if(task)
    {
        bool pushTaskToWorld = false;

        if (task->m_TaskType == PENDINGTASK_ADD)
        {
            //Adding, so make sure to check its state and update accordingly.
            switch (((PendingTaskAdd*)task)->m_actor->GetActorState())
            {
            case IActor::ActorState_None:
            case IActor::ActorState_Unloaded:
                XCASSERT(false);

            case IActor::ActorState_Loading:
                ((PendingTaskAdd*)task)->m_actor->Load();
                pushTaskToWorld = true;
                break;

            case IActor::ActorState_Loaded:
                pushTaskToWorld = true;
                break;

            default:
                break;
            }
        }
        else if (task->m_TaskType == PENDINGTASK_REMOVE)
        {
            pushTaskToWorld = true;
        }

        if (pushTaskToWorld)
        {
            //Move the task to the buffered task which will be process from world main thread which is actual add/remove from the world actors list.
            m_pendingTaskLock.Enter();
            m_bufferedPendingTaskList.push_back(task);
            m_pendingTaskLock.Exit();

            pushTaskToWorld = false;
        }

        task = nullptr;
    }
}

void WorldPendingTasks::ProcessFromWorld()
{
    //Call from Main World Thread.
    bool isEmpty = false;
    IPendingTask* task;

    while (!isEmpty)
    {
        m_pendingTaskLock.Enter();
        
        if (m_bufferedPendingTaskList.size() <= 0)
        {
            isEmpty = true;
        }
        else
        {
            task = m_bufferedPendingTaskList.back();
            m_bufferedPendingTaskList.pop_back();
        }

        m_pendingTaskLock.Exit();

        if (task != nullptr)
        {
            switch (task->m_TaskType)
            {
                case PENDINGTASK_ADD:
                    m_parentWorld.AddActor(((PendingTaskAdd*)task)->m_actor);
                    break;
                case PENDINGTASK_REMOVE:
                    m_parentWorld.RemoveActor(((PendingTaskRemove*)task)->m_actorId);
                    break;
            }

            XCDELETE(task);
            task = nullptr;
        }
    }
}

void WorldPendingTasks::Destroy()
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

void World::Update(f32 dt)
{
    if (m_addRemoveFlag.load(std::memory_order_acquire))
    {
        if (m_worldPendingTasks->HasBufferedPendingTasks())
        {
            m_worldPendingTasks->ProcessFromWorld();
        }
        m_addRemoveFlag.store(false, std::memory_order_release);
    }

    for (auto& gameObject : m_GameObjects)
    {
        if (gameObject.second->IsInvalidated())
        {
            RequestRemoveActor(gameObject.second->GetBaseObjectId());
        }
        else if(gameObject.second->IsWorldReady() && gameObject.second->GetActorState() == IActor::ActorState_Loaded)
        {
            gameObject.second->Update(dt);
        }
        else
        {
            gameObject.second->UpdateState();
        }
    }
}

void World::Draw(XC_Graphics& graphicsSystem)
{
    //Nothing here. Drawing of actors done in RenderingPool.
}

void World::OnEvent(IEvent* evt)
{
    Event_World* evtWorld = dynamic_cast<Event_World*>(evt);
    
    if (evtWorld)
    {
        if (evtWorld->GetEventType() == EventType_WorldReady)
        {
            SetWorldReady(true);
        }
    }
}

void World::RequestAddActor(IActor* actor)
{
    WorldPendingTasks::IPendingTask* task = XCNEW(WorldPendingTasks::PendingTaskAdd)(WorldPendingTasks::PENDINGTASK_ADD, std::move(actor));
    m_worldPendingTasks->AddTask(task);
}

void World::AddActor(IActor* actor)
{
    i32 actorId = actor->GetBaseObjectId();
    Logger("[WORLD] Adding actor id : %d", actorId);

    //Check if it's physics actor
    PhysicsActor* phyActor = dynamic_cast<PhysicsActor*>(actor);
    if (phyActor != nullptr)
    {
        m_PhysicsActorIDs.push_back(actorId);
    }

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

    m_GameObjects[actorId] = actor;

    //Register with renderpool
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddRenderableObject((IRenderableObject*)m_GameObjects[actorId], actorId);

    //m_GameObjects[actorId]->SetWorldReady(true);
}

void World::RequestRemoveActor(i32 key)
{
    WorldPendingTasks::IPendingTask* task = XCNEW(WorldPendingTasks::PendingTaskRemove)(WorldPendingTasks::PENDINGTASK_REMOVE, key);
    m_worldPendingTasks->AddTask(task);
}

void World::RemoveActor(i32 _key)
{
    std::map<i32, IActor*>::iterator it;
    it = m_GameObjects.find(_key);

    if ( it != m_GameObjects.end())
    {
        Logger("[WORLD] Removing actor id : %d", _key);

        //Unregister from rendering pool 
        XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
        graphicsSystem.GetRenderingPool().RemoveRenderableObject((IRenderableObject*)it->second, it->second->GetBaseObjectId());

        //Remove from the filtering vectors
        removeKey(m_PhysicsActorIDs, _key);
        removeKey(m_PlayableCharacterActors, _key);
        removeKey(m_NonPlayableCharacterActors, _key);

        it->second->Unload();
        it->second->Destroy();

        XCDELETE(it->second);
        m_GameObjects.erase(it);
    }
}

bool World::IsActorReady(i32 actorId)
{
    if (m_GameObjects.find(actorId) != m_GameObjects.end())
    {
       return m_GameObjects[actorId]->IsWorldReady();
    }
    
    //Not ready yet. So just return false.
    return false;
}

void World::removeKey(std::vector<i32>& list, i32 key)
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

void World::Destroy()
{
    m_worldPendingTasks->Destroy();
    m_taskManager->UnregisterTask(m_worldPendingTasks->GetThreadId());

#if !defined(EDITOR)
    m_worldCollisionTask->Destroy();
    m_taskManager->UnregisterTask(m_worldCollisionTask->GetThreadId());
#endif

    m_worldReady = false;

    //Remove all actors
    for (auto& actor : m_GameObjects)
    {
        actor.second->Unload();
        actor.second->Destroy();
        XCDELETE(actor.second);
    }

    m_PlayableCharacterActors.clear();
    m_NonPlayableCharacterActors.clear();
    m_PhysicsActorIDs.clear();
    m_GameObjects.clear();
}

void World::CheckAllCollisions()
{
    if (!m_addRemoveFlag.load(std::memory_order_acquire))
    {
        if (!IsWorldQuiting() && m_worldReady)
        {
            if (m_PhysicsActorIDs.size() > 0)
            {
                std::vector<i32>::iterator obj1;
                for (obj1 = m_PhysicsActorIDs.begin(); obj1 != m_PhysicsActorIDs.end() - 1; ++obj1)
                {
                    std::vector<i32>::iterator obj2;
                    for (obj2 = obj1 + 1; obj2 != m_PhysicsActorIDs.end(); ++obj2)
                    {
                        if (*obj1 && *obj2 && CheckCollision((PhysicsActor*)m_GameObjects[*obj1], (PhysicsActor*)m_GameObjects[*obj2]))
                        {
                            //Logger("COLLIDED VERY WELL");
                        }
                    }
                }
            }
        }
        m_addRemoveFlag.store(true, std::memory_order_release);
    }
}

bool World::CheckCollision(PhysicsActor* obj1, PhysicsActor* obj2)
{
    switch (obj1->GetCollisionDetectionType() | obj2->GetCollisionDetectionType())
    {
        case COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
            if (obj1->GetBoundBox()->m_TransformedBox.Intersects(obj2->GetBoundBox()->m_TransformedBox))
            {
                //Before resolving know which corner point was hit
                DirectX::XMFLOAT3 points[8];
                obj1->GetBoundBox()->m_TransformedBox.GetCorners(points);

                XCVec4 contactNormal;

                contactNormal = CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(obj1->GetBoundBox(), obj2->GetBoundBox());
                //contactNormal = getContactNormalFromBoundBoxContainedPlane(obj2->getMesh()->getAABoundBox(), obj1->getMesh()->getAABoundBox());
                //Resolve the collision
                m_particleContact.ContactResolve(obj1, obj2, 1.0f, 0.0f, contactNormal);
                return true;
            }
            break;

        case COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX | COLLISIONDETECTIONTYPE_TRIANGLE:

            break;

        case COLLISIONDETECTIONTYPE_TERRAIN | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
            {
                //Handle terrain and boundbox collision
                PhysicsActor* bboxActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX ? obj1 : obj2;
                PhysicsActor* terrainActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_TERRAIN ? obj1 : obj2;

                //XMVECTOR contactPoint = getTerrainPointOfContactWithBoundBox(bboxActor, terrainActor);
                XCVec4 contactPoint = ((Terrain*)terrainActor)->CheckTerrainCollisionFromPoint(bboxActor->GetBoundBox());

                //Resolve the collision
                //PARTICLE_CONTACT->ContactResolve(bboxActor, terrainActor, 1.0f, 0.2f, contactNormal);
                if (!IsVectorEqual(contactPoint, XCVec4::XCFloat4ZeroVector))
                {
                    //This is a impulse particle contact resolver, it actually first puts an object on a vertex
                    XCVec4 direction = XCVec4(0.0f, (f32)(contactPoint.Get<Y>()) + (f32)0.1, 0.0f, 0.0f);
                    m_particleContact.ApplyImpulse(bboxActor, direction);

                    //This I am trying now to make it work with existing collision resolver, works perfectly.
                    XCVec4 up = XCVec4(0.0f, 1.0f, 0.0f, 0.0f);
                    m_particleContact.ContactResolve(bboxActor, terrainActor, 1.0f, 0.2f, up);

                    //Logger("[Terrain Collision] Hit with %s with contact point %f \n", bboxActor->getMesh()->getUserFriendlyName().c_str(), XMVectorGetY(contactPoint));
                }
                return true;

                break;
            }

        case COLLISIONDETECTIONTYPE_BULLET | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
            {
                PhysicsActor* objectActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX ? obj1 : obj2;
                PhysicsActor* bulletActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_BULLET ? obj1 : obj2;

                DirectX::ContainmentType type = objectActor->GetBoundBox()->m_TransformedBox.Contains(bulletActor->GetBoundBox()->m_TransformedBox);

                if (type == DirectX::CONTAINS || type == DirectX::INTERSECTS)
                {
                    //Before resolving know which corner point was hit
                    DirectX::XMFLOAT3 points[8];
                    obj1->GetBoundBox()->m_TransformedBox.GetCorners(points);

                    XCVec4 contactNormal;

                    contactNormal = CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(obj1->GetBoundBox(), obj2->GetBoundBox());

                    //Resolve the collision
                    m_particleContact.ContactResolve(obj1, obj2, 1.0f, 0.5f, contactNormal);

                    objectActor->Invalidate();
                    bulletActor->Invalidate();

                    Logger("[BULLET HIT] Hit Obj1 : %d Obj 2: %d", obj1->GetCollisionDetectionType(), obj2->GetCollisionDetectionType());

                    return true;
                }
            }
        default:
            break;
    }

    return false;
}



IActor* World::GetActor(i32 _InstanceID)
{
    return m_GameObjects[_InstanceID];
}

i32 World::GetInstanceIdOfActor(IActor* actor)
{
    for (auto& gameObject : m_GameObjects)
    {
        if (gameObject.second == actor)
        {
            return gameObject.first;
        }
    }

    return -1;
}

void World::SetMainPlayableCharacter(i32 instanceId)
{ 
    if (m_GameObjects[instanceId] != nullptr)
    {
        //First disable current main playable character,
        PlayableCharacterActor* pcActor = dynamic_cast<PlayableCharacterActor*>(m_GameObjects[m_currentMainPlayableCharacter]);
        if (pcActor != nullptr)
        {
            pcActor->setControl(false);
        }

        //Next set the new playable character
        m_currentMainPlayableCharacter = instanceId;
        pcActor = dynamic_cast<PlayableCharacterActor*>(m_GameObjects[m_currentMainPlayableCharacter]);
        if (pcActor != nullptr)
        {
            pcActor->setControl(true);
        }
    }
}

i32	World::GetNextPlayableActor(i32 instanceId)
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