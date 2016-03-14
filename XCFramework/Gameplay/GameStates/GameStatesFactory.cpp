/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "GameStatesFactory.h"

#include "Gameplay/GameStates/EditorStates/EditorLoadingState.h"
#include "Gameplay/GameStates/EditorStates/EditorRunningState.h"

#include "Gameplay/GameStates/IntroState.h"
#include "Gameplay/GameStates/MainMenuState.h"
#include "Gameplay/GameStates/RunningState.h"
#include "Gameplay/GameStates/PauseState.h"
#include "Gameplay/GameStates/NewGameState.h"
#include "Gameplay/GameStates/EndGameState.h"
#include "Gameplay/GameStates/LoadingState.h"
#include "Gameplay/GameStates/LoadingWorldState.h"

GameStatesFactory::GameStatesFactory()
{
    m_statesCount = 0;
}


GameStatesFactory::~GameStatesFactory()
{
}

void GameStatesFactory::InitFactory()
{
    RegisterStates();
}

void GameStatesFactory::RegisterStates()
{
    RegisterObject<IntroState>("IntroState");
    RegisterObject<LoadingState>("LoadingState");
    RegisterObject<LoadingWorldState>("LoadingWorldState");
    RegisterObject<MainMenuState>("MainMenuState");
    RegisterObject<NewGameState>("NewGameState");
    RegisterObject<PauseState>("PauseState");
    RegisterObject<RunningState>("RunningState");
    RegisterObject<EndGameState>("EndGameState");

#if defined(EDITOR)
    RegisterObject<EditorLoadingState>("EditorLoadingState");
    RegisterObject<EditorRunningState>("EditorRunningState");
#endif
}


IGameState* GameStatesFactory::CreateState(std::string stateName)
{
    std::unique_lock<std::mutex> m(m_gameStatesFactoryLock);

    IGameState* state = (IGameState*) CreateObject(stateName);

    Logger("[GAME STATES FACTORY] State : %s  ID : %d created", stateName.c_str(), m_statesCount);

    m.unlock();

    return state;
}


void GameStatesFactory::LoadStates(FILE* packageFile, IGameState* const state)
{
    //char rscPath[256];
  
    //fscanf(packageFile, "%s", rscPath);
    //Based on every resources init parameters, load it.
    /*if (resource->getResourceType() == RESOURCETYPE_TEXTURE2D)
    {
        resource->load(rscPath);
    }
    else if (resource->getResourceType() == RESOURCETYPE_MESH)
    {
        float scaling = 0.0f;
        fscanf(packageFile, "%f", &scaling);
        ((XCMesh*)resource)->load(rscPath, scaling);
    }*/
}

void GameStatesFactory::DestroyFactory()
{
    m_registeredStates.clear();
}


#if defined(EDITOR)

extern "C" __declspec(dllexport) void GetAllStateTypes(void* info)
{
}

extern "C" __declspec(dllexport) int GetNoOfStateTypes()
{
    return 0;
}

#endif