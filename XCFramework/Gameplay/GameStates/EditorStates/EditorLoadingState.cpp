#include "stdafx.h"

#include "Gameplay/GameStates/EditorStates/EditorLoadingState.h"
#include "Gameplay/GameStates/GameStateTypes.h"

#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/TaskManager/TaskManager.h"

using namespace GameState;

EditorLoadingState::EditorLoadingState(void)
{
    m_isPaused = false;
}

EditorLoadingState::~EditorLoadingState(void)
{
}

void GameState::EditorLoadingState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);

    ResourceManager& resMgr = (ResourceManager&) SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_taskMgr = (TaskManager*) &SystemLocator::GetInstance()->RequestSystem("TaskManager");

    //Load the resources here.
    m_loadTaskId = resMgr.LoadResourcesFromPackage("Assets\\Packages\\Resources.pak");
}

void EditorLoadingState::Update(float dt)
{
    //Wait for resource loader to complete the loading of resources. When done move to next state.
    if (m_taskMgr->GetTaskState(m_loadTaskId) == TASKSTATE_DESTROY)
    {
        Event_GameStateChange event("EditorRunningState", STATE_DESTROY);
        EventBroadcaster& broadcaster = (EventBroadcaster&) SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
        broadcaster.BroadcastEvent(&event);
    }
}

void GameState::EditorLoadingState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);

    float variadic = (float)(rand() % 5) / 100;
    graphicsSystem.SetClearColor(XCVec4(0.5f, variadic, 0.5f, 1.0f));
}

void EditorLoadingState::Destroy()
{
    IGameState::Destroy();

    XC_Graphics& graphicsSystem = (XC_Graphics&) SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    graphicsSystem.SetClearColor(XCVec4(1.0f, 1.0f, 1.0f, 1.0f));
}