/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/MainMenuState.h"

using namespace GameState;

MainMenuState::MainMenuState(void)
{
    m_isPaused = false;
}

MainMenuState::~MainMenuState(void)
{
}

void MainMenuState::Init()
{
    IGameState::Init();
}

void MainMenuState::Update(f32 dt)
{
    Event_GameStateChange event("NewGameState", STATE_DESTROY);
    EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadcaster.BroadcastEvent(&event);
}

void GameState::MainMenuState::Draw(XCGraphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void MainMenuState::Destroy()
{
    IGameState::Destroy();
}