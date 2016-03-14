/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/MainMenuState.h"
#include "Gameplay/GameFiniteStateMachine.h"

using namespace GameState;

MainMenuState::MainMenuState(void)
{
    m_isPaused = false;
}

MainMenuState::~MainMenuState(void)
{
}

void MainMenuState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);
}

void MainMenuState::Update(float dt)
{
    m_gameFSM->SetState("NewGameState", STATE_DESTROY);
}

void GameState::MainMenuState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void MainMenuState::Destroy()
{
    IGameState::Destroy();
}