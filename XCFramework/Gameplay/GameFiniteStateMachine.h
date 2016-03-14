/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Gameplay/GameStates/IGameState.h"
#include "GameStates/GameStateTypes.h"

#include "Gameplay/World.h"
#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Gameplay/GameStates/GameStatesFactory.h"

using namespace GameState;

class GameFiniteStateMachine : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(GameFiniteStateMachine)

    GameFiniteStateMachine();
    ~GameFiniteStateMachine(void);

    void                        Init();
    void                        Update(float dt);
    void                        Draw(XC_Graphics& graphicsSystem);
    void                        Destroy();

    void                        SetState(std::string stateName, EWhatToDoWithPreviousState what = STATE_NONE);
    std::string                 GetCurrentState();

    void                        SetGameExit(bool value);
    bool                        GetGameExit();

    IGameState&                 GetCurrentGameState() { return (IGameState&)m_StateStack.back(); }

    GameStatesFactory&          GetGameStateFactory() { return *m_gameStateFactory; }
    GameActorsFactory&          GetGameActorsFactory() { return *m_gameActorFactory; }
    World&                      GetWorldSystem() { return *m_worldSystem; }

private:

    std::vector<IGameState*>    m_StateStack;

    bool                        m_IsGameExit;
    std::string                 m_CurrentGameState;

    void                        PushState(std::string stateName);
    IGameState*                 PopState();

    GameStatesFactory*          m_gameStateFactory;
    GameActorsFactory*          m_gameActorFactory;
    World*                      m_worldSystem;
};