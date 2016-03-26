/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Engine/Input/Directinput.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Graphics/XC_Lighting/XC_LightManager.h"
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
        void                    Update(float dt);
        void                    Draw(XC_Graphics& graphicsSystem);
        void                    Destroy();

    private:
        int                     m_currentMainPlayableActor;
        bool                    m_bMainPlayerSet;

        XC_LightManager*        m_lightManagerSystem;
        XC_CameraManager*       m_cameraSystem;
        DirectInput*            m_directInput;
        World*                  m_worldSystem;
    };
}