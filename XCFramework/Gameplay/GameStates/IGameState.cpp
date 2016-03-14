/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/IGameState.h"

using namespace GameState;

IGameState::IGameState(void)
{
    m_isPaused = false;
}


IGameState::~IGameState(void)
{
}

void GameState::IGameState::Init(GameFiniteStateMachine& gameFSM)
{
    m_gameFSM = &gameFSM;
}

void IGameState::Draw(XC_Graphics& graphicsSystem)
{

}

void IGameState::Destroy()
{

}