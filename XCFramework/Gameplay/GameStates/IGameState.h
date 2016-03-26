/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/IBase.h"
#include "Engine/Event/IEvent.h"
#include "Engine/Event/EventBroadcaster.h"

#include "Gameplay/GameStates/GameStateTypes.h"

class XC_Graphics;

namespace GameState
{
    class IGameState : public IBase
    {
    public:
        IGameState(void);
        virtual ~IGameState(void);

        virtual void            Init();
        virtual void            Update(float dt) = 0;
        virtual void            Draw(XC_Graphics& graphicsSystem);
        virtual void            Destroy();

        bool                    GetPauseState() { return m_isPaused; }

    protected:
        bool                    m_isPaused;
    };
}

class Event_GameStateChange : public IEvent
{
public:
    Event_GameStateChange(std::string stateName, EWhatToDoWithPreviousState prevState)
        : IEvent(EventType_GameStateChange)
        , m_stateName(stateName)
        , m_previousState(prevState)
    {
    }

    std::string                 m_stateName;
    EWhatToDoWithPreviousState  m_previousState;
};