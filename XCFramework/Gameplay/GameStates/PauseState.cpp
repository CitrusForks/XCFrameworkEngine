/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/PauseState.h"


using namespace GameState;


PauseState::PauseState(void)
{
    m_isPaused = false;
}

PauseState::~PauseState(void)
{
}

void GameState::PauseState::Init()
{
    IGameState::Init();
}

void PauseState::Update(f32 dt)
{
}

void GameState::PauseState::Draw(XCGraphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void PauseState::Destroy()
{
    IGameState::Destroy();
}