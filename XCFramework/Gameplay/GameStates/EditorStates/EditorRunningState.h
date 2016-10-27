#pragma once

#include "Gameplay/GameStates/IGameState.h"

#include "Graphics/XCLighting/XCLightManager.h"

class WorldEditor;
class GameActorsFactory;

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
        void                        Draw(XCGraphics& graphicsSystem);
        void                        Destroy();

    private:
        XCLightManager*             m_lightManagerSystem;
        GameActorsFactory*          m_gameActorFactory;
        WorldEditor*                m_worldEditor;
    };

}