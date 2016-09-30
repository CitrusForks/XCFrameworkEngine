/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/LoadingState.h"
#include "Gameplay/GameStates/GameStateTypes.h"

#include "Engine/TaskManager/Task/ITask.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/LoadPackageFileFBTask.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/BasicGeometry/MeshGeneratorSystem.h"

#include "Assets/Packages/PackageConsts.h"

using namespace GameState;

LoadingState::LoadingState(void)
{
}

LoadingState::~LoadingState(void)
{
    if (m_loadPackageTask)
    {
        XCDELETE(m_loadPackageTask);
    }
}

void LoadingState::Init()
{
    IGameState::Init();

    TaskManager& taskMgr = SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");

    m_loadPackageTask = XCNEW(LoadPackageFileFBTask)(RESOURCE_DATA_FILEPATH);
    m_futurePackageLoaded = taskMgr.RegisterTask(m_loadPackageTask);
}

void LoadingState::Update(f32 dt)
{
    //Wait for resource loader to complete the loading of resources. When done move to next state.
    if (m_futurePackageLoaded._Is_ready() /*&& m_futurePackageLoaded.wait_for(std::chrono::seconds(1)) == std::future_status::ready */)
    {
        if(m_futurePackageLoaded.valid() && m_futurePackageLoaded.get() > 0)
        {
            //Load the Runtime Mesh Generator to load the basic meshes.
            SystemContainer& container = (SystemContainer&)SystemLocator::GetInstance()->GetSystemContainer();
            container.RegisterSystem<MeshGeneratorSystem>("MeshGenerator");

            MeshGeneratorSystem& meshGenerator = (MeshGeneratorSystem&)container.CreateNewSystem("MeshGenerator");
            meshGenerator.Load();

            container.RemoveSystem("MeshGenerator");

            Event_GameStateChange event("LoadingWorldState", STATE_DESTROY);
            EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
            broadcaster.BroadcastEvent(&event);
        }
    }
}

void GameState::LoadingState::Draw(XCGraphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);

    //float variadic = (float)(rand() % 5) / 10;
    //graphicsSystem.SetClearColor(XMFLOAT4(variadic, 0.5f, 0.5f, 1.0f));
}

void LoadingState::Destroy()
{
    IGameState::Destroy();

    TaskManager& taskMgr = SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");
    taskMgr.UnregisterTask(m_loadPackageTask->GetThreadId());

    //Keep the task in memory until this LoadingState object is deleted. This is beacuse, the flat buffer instance remains until the its parent object exists.
    //In LoadPackageFileFBTask we have already requested to load and the fb pointer remains active until the object is deleted. So if we delete here, 
    //then the buffer pointers to every resource load request becomes dangling, as fb pointer is released.
    //XCDELETE(m_loadPackageTask);
}