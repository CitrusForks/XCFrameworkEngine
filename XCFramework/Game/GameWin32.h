/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Win32/AppFrameworkWin32.h"

//Forward Declarations
class SystemContainer;
class MemorySystem;
class EventBroadcaster;
class TaskManager;
class FlatBuffersSystem;
class XCGraphics;
class XCInput;
class ResourceManager;
class XCCameraManager;
class XCLightManager;
class GameFiniteStateMachine;
class NetworkManager;
class INetPeer;

class GameWin32 : public AppFrameworkWin32
{
public:
    GameWin32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    virtual ~GameWin32();

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
    XCGraphics*                 m_graphicsSystem;
    XCInput*                 m_directInputSystem;
    ResourceManager*             m_resourceManagingSystem;
    XCCameraManager*            m_cameraManagingSystem;
    XCLightManager*             m_lightsSystem;
    GameFiniteStateMachine*      m_gameFSM;
    NetworkManager*              m_networkManagingSystem;
    INetPeer*                    m_liveDriveClient;
};