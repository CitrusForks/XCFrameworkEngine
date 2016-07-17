/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/NewGameState.h"

using namespace GameState;

NewGameState::NewGameState(void)
{
    m_isPaused = false;
}


NewGameState::~NewGameState(void)
{
}

void NewGameState::Init()
{
    IGameState::Init();
}

void NewGameState::Update(f32 dt)
{
    Event_GameStateChange event("LoadingState", STATE_DESTROY);
    EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadcaster.BroadcastEvent(&event);
}

void NewGameState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void NewGameState::Destroy()
{
    IGameState::Destroy();
}