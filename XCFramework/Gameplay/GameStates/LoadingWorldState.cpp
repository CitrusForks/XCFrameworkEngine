/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/LoadingWorldState.h"
#include "Gameplay/GameStates/GameStateTypes.h"

#include "Engine/TaskManager/TaskManager.h"
#include "Engine/GameplayBase/SceneLoader.h"
#include "Engine/GameplayBase/WorldEventTypes.h"

#include "Assets/Packages/PackageConsts.h"

using namespace GameState;

LoadingWorldState::LoadingWorldState(void)
{
}

LoadingWorldState::~LoadingWorldState(void)
{
}

void GameState::LoadingWorldState::Init()
{
    IGameState::Init();
    SceneGraph& world = SystemLocator::GetInstance()->RequestSystem<SceneGraph>("SceneGraph");

    m_sceneLoader = XCNEW(SceneLoader)(world, SCENE_DATA_FILEPATH);

    TaskManager& taskMgr = SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");
    m_futureSceneLoaded = taskMgr.RegisterTask(m_sceneLoader);
}

void LoadingWorldState::Update(f32 dt)
{
    //Wait for resource loader to complete the loading of resources. When done move to next state.
    if (m_futureSceneLoaded._Is_ready() && m_futureSceneLoaded.valid() && m_futureSceneLoaded.get() > 0)
    {
        EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");

        Event_Scene event(EventType_SceneReady);
        broadcaster.BroadcastEvent(&event);

        Event_GameStateChange gsEvent("RunningState", STATE_DESTROY);
        broadcaster.BroadcastEvent(&gsEvent);
    }
}

void GameState::LoadingWorldState::Draw(XCGraphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void LoadingWorldState::Destroy()
{
    IGameState::Destroy();

    TaskManager& taskMgr = SystemLocator::GetInstance()->RequestSystem<TaskManager>("TaskManager");
    taskMgr.UnregisterTask(m_sceneLoader->GetThreadId());
    
    XCDELETE(m_sceneLoader);

}