/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameStates/LoadingState.h"
#include "Engine/Resource/ResourceManager.h"
#include "Gameplay/GameStates/GameStateTypes.h"

#include "Gameplay/GameFiniteStateMachine.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Engine/TaskManager/TaskManager.h"

#include "Assets/Packages/PackageConsts.h"

using namespace GameState;

LoadingState::LoadingState(void)
{
}

LoadingState::~LoadingState(void)
{
}

void GameState::LoadingState::Init(GameFiniteStateMachine& gameFSM)
{
    IGameState::Init(gameFSM);

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");

    //Load the resources here.
    m_taskId = resMgr.LoadResourcesFromPackageFB(RESOURCE_DATA_FILEPATH);
}

void LoadingState::Update(float dt)
{
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");

    //Wait for resource loader to complete the loading of resources. When done move to next state.
    if ( resMgr.IsPackageLoaded())
    {
        m_gameFSM->SetState("LoadingWorldState", STATE_DESTROY);
    }
}

void GameState::LoadingState::Draw(XC_Graphics& graphicsSystem)
{
    IGameState::Draw(graphicsSystem);

    //float variadic = (float)(rand() % 5) / 10;
    //graphicsSystem.SetClearColor(XMFLOAT4(variadic, 0.5f, 0.5f, 1.0f));
}

void LoadingState::Destroy()
{
    IGameState::Destroy();
}