/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/IBase.h"

class GameFiniteStateMachine;
class XC_Graphics;

namespace GameState
{
    class IGameState : public IBase
    {
    public:
        IGameState(void);
        virtual ~IGameState(void);

        virtual void            Init(GameFiniteStateMachine& gameFSM);
        virtual void            Update(float dt) = 0;
        virtual void            Draw(XC_Graphics& graphicsSystem);
        virtual void            Destroy();

        bool                    GetPauseState() { return m_isPaused; }

    protected:
        bool                    m_isPaused;
        GameFiniteStateMachine* m_gameFSM;
    };

}