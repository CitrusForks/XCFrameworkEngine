/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Engine/TaskManager/Task/ITask.h"

namespace GameState
{
    class LoadingState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(LoadingState)

        LoadingState(void);
        virtual ~LoadingState(void);

        void                    Init();
        void                    Update(f32 dt);
        void                    Draw(XC_Graphics& graphicsSystem);
        void                    Destroy();

    private:                    
        u64                     m_taskId;
        std::future<i32>        m_futurePackageLoaded;
        ITask*                  m_loadPackageTask;
    };
}