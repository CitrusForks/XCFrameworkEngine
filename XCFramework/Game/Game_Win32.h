/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/ApplicationFramework/Win32/AppFramework_Win32.h"

#include "Engine/Event/EventBroadcaster.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Input/Directinput.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Engine/Resource/ResourceManager.h"

#include "Network/INetPeer.h"
#include "Network/NetworkManager.h"

#include "Graphics/XC_Graphics.h"

#include "Gameplay/GameFiniteStateMachine.h"
#include "Gameplay/XC_Camera/XC_CameraManager.h"

class Game_Win32 : public AppFramework_Win32
{
public:
    Game_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    virtual ~Game_Win32();

    int                          Init();
    void                         Update(float dt);
    void                         Draw();
    void                         Destroy();

    void                         RegisterSystems();
    void                         OnResize();
    void                         EnableFullScreenMode(bool enable);

private:
    
    EventBroadcaster*            m_eventBroadcaster;
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