/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameStates/IntroState.h"

using namespace GameState;

IntroState::IntroState(void)
{
    m_isPaused	 = false;
}

IntroState::~IntroState(void)
{
}

void IntroState::Init()
{
    IGameState::Init();
}

void IntroState::Update(f32 dt)
{
    Logger("[GAME STATE] Intro");
    Event_GameStateChange event("MainMenuState", STATE_DESTROY);
    EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadcaster.BroadcastEvent(&event);
}

void IntroState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);
}

void IntroState::Destroy()
{
    IGameState::Destroy();
}