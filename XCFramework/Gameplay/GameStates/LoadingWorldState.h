/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Gameplay/WorldSceneLoader.h"

namespace GameState
{
    class LoadingWorldState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(LoadingWorldState)

        LoadingWorldState(void);
        virtual ~LoadingWorldState(void);

        void                    Init();
        void                    Update(f32 dt);
        void                    Draw(XC_Graphics& graphicsSystem);
        void                    Destroy();

    private:

        u64           m_taskId;
        WorldSceneLoader*       m_worldLoader;
        std::future<i32>        m_futureWorldLoaded;
    };
}