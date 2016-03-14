/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/IntroState.h"
#include "Gameplay/GameFiniteStateMachine.h"

using namespace GameState;

IntroState::IntroState(void)
{
    m_isPaused	 = false;
}

IntroState::~IntroState(void)
{
}

void GameState::IntroState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);
}

void IntroState::Update(float dt)
{
    Logger("[GAME STATE] Intro");
    m_gameFSM->SetState("MainMenuState", STATE_DESTROY);
}

void GameState::IntroState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void IntroState::Destroy()
{
    IGameState::Destroy();
}