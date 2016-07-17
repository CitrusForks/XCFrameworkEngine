/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/ObjectFactory.h"
#include "Engine/System/ISystem.h"
#include "IGameState.h"
#include "GameStateTypes.h"

using namespace GameState;

class GameStatesFactory: ObjectFactory, public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(GameStatesFactory)

    GameStatesFactory();
    virtual ~GameStatesFactory();

    void                            InitFactory()       override final;
    void                            DestroyFactory()    override final;

    void                            RegisterStates();

    IGameState*                     CreateState(std::string userFriendlyName);
    void                            LoadStates(FILE* packageFile, IGameState* const gameState);

private:

    i32                                  m_statesCount;
    std::map<EGameState, std::string>    m_registeredStates;
    std::mutex                           m_gameStatesFactoryLock;
};