#pragma once

#include "Gameplay/GameStates/IGameState.h"
#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Engine/Graphics/XC_Lighting/XC_LightManager.h"

class WorldEditor;

namespace GameState
{

    class EditorRunningState : public IGameState
    {
    public:
        DECLARE_OBJECT_CREATION(EditorRunningState)

        EditorRunningState(void);
        virtual ~EditorRunningState(void);

        void                        Init();
        void                        Update(float dt);
        void                        Draw(XC_Graphics& graphicsSystem);
        void                        Destroy();

    private:
        XC_LightManager*            m_lightManagerSystem;
        GameActorsFactory*          m_gameActorFactory;
        WorldEditor*                m_worldEditor;
    };

}