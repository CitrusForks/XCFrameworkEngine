/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/NewGameState.h"
#include "Gameplay/GameFiniteStateMachine.h"

using namespace GameState;


NewGameState::NewGameState(void)
{
    m_isPaused = false;
}


NewGameState::~NewGameState(void)
{
}

void GameState::NewGameState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);
}

void NewGameState::Update(float dt)
{
    m_gameFSM->SetState("LoadingState", STATE_DESTROY);
}

void NewGameState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void NewGameState::Destroy()
{
    IGameState::Destroy();
}