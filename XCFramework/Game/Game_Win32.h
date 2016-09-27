/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/ApplicationFramework/Win32/AppFramework_Win32.h"

//Forward Declarations
class SystemContainer;
class MemorySystem;
class EventBroadcaster;
class TaskManager;
class FlatBuffersSystem;
class XC_Graphics;
class DirectInput;
class ResourceManager;
class XC_CameraManager;
class XC_LightManager;
class GameFiniteStateMachine;
class NetworkManager;
class INetPeer;

class Game_Win32 : public AppFramework_Win32
{
public:
    Game_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    virtual ~Game_Win32();

    i32                          Init()         override;
    void                         Update(f32 dt) override;
    void                         Draw()         override;
    void                         Destroy()      override;

    void                         OnResize()     override;
    void                         EnableFullScreenMode(bool enable) override;

protected:
    void                         RegisterSystems();

private:

    SystemContainer*             m_systemContainer;
    MemorySystem*                m_memorySystem;
    EventBroadcaster*            m_eventBroadcaster;
    TaskManager*                 m_taskManagingSystem;
    FlatBuffersSystem*           m_fbSystem;
    XC_Graphics*                 m_graphicsSystem;
    DirectInput*                 m_directInputSystem;
    ResourceManager*             m_resourceManagingSystem;
    XC_CameraManager*            m_cameraManagingSystem;
    XC_LightManager*             m_lightsSystem;
    GameFiniteStateMachine*      m_gameFSM;
    NetworkManager*              m_networkManagingSystem;
    INetPeer*                    m_liveDriveClient;
};