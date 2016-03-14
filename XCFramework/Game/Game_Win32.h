/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/ApplicationFramework/Win32/AppFramework_Win32.h"
#include "Engine/Utils/Timer.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Input/Directinput.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Network/INetPeer.h"
#include "Engine/Network/NetworkManager.h"

#include "Gameplay/GameFiniteStateMachine.h"

class Game_Win32 : public AppFramework_Win32
{
public:
    Game_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    ~Game_Win32();

    int                     Init();
    void                    OnResize();
    void                    UpdateScene(float dt);
    void                    DrawScene();
    void                    EnableFullScreenMode(bool enable);
    void                    Destroy();

    void                    RegisterSystems();

private:
    Timer*                       m_timer;
    INetPeer*                    m_liveDriveClient;
    DirectInput*                 m_directInputSystem;
    XC_Graphics*                 m_graphicsSystem;
    FlatBuffersSystem*           m_fbSystem;
    ResourceManager*             m_resourceManagingSystem;
    TaskManager*                 m_taskManagingSystem;
    XC_CameraManager*            m_cameraManagingSystem;
    GameFiniteStateMachine*      m_gameFSM;
    NetworkManager*              m_networkManagingSystem;

    SystemContainer*             m_systemContainer;
};