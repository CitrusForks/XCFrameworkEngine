/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"

class SceneLoader;

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
        void                    Draw(XCGraphics& graphicsSystem);
        void                    Destroy();

    private:
        u64                     m_taskId;
        SceneLoader*            m_sceneLoader;
        std::future<i32>        m_futureSceneLoaded;
    };
}