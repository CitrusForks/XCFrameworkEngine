#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Engine/TaskManager/TaskManager.h"

namespace GameState
{
    class EditorLoadingState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(EditorLoadingState)

        EditorLoadingState(void);
        virtual ~EditorLoadingState(void);

        void                        Init();
        void                        Update(float dt);
        void                        Draw(XC_Graphics& graphicsSystem);
        void                        Destroy();

    private:
        int                         m_loadTaskId;
        TaskManager*                m_taskMgr;
    };
}