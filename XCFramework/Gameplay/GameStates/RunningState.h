/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Engine/Input/XCInput.h"
#include "Gameplay/XCCamera/XCCameraManager.h"
#include "Graphics/XCLighting/XCLightManager.h"
#include "Gameplay/World.h"

namespace GameState
{
    class RunningState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(RunningState)

        RunningState(void);
        virtual ~RunningState(void);

        void                    Init();
        void                    Update(f32 dt);
        void                    Draw(XCGraphics& graphicsSystem);
        void                    Destroy();

    private:
        i32                     m_currentMainPlayableActor;
        bool                    m_bMainPlayerSet;

        XCCameraManager*       m_cameraSystem;
        XCInput*            m_directInput;
        World*                  m_worldSystem;
    };
}