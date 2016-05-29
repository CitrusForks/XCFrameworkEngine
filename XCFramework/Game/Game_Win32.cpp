/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Game_Win32.h"
#include "Assets/Packages/PackageConsts.h"
#include "Engine/System/SystemLocator.h"
#include "Engine/Network/Clients/LiveDriveVRClient.h"
#include "Engine/Input/Directinput_Win32.h"

#if defined(XCGRAPHICS_DX12)
#include "Engine/Graphics/XC_GraphicsDx12.h"
#elif defined(XCGRAPHICS_DX11)
#include "Engine/Graphics/XC_GraphicsDx11.h"
#elif defined(XCGRAPHICS_GL)
#include "Engine/Graphics/XC_GraphicsGL.h"
#endif

Game_Win32::Game_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa)
    : AppFramework_Win32(hInstance, winCaption, enable4xMsaa)
{
    srand(time_t(0));
    Init();

    Logger("[GAME MAIN]");
}

Game_Win32::~Game_Win32()
{
}

int Game_Win32::Init()
{
    AppFramework_Win32::Init();

    //Register Systems
    RegisterSystems();

    //Timer
    m_timer = (Timer*)&m_systemContainer->CreateNewSystem("Timer");

    //Event Broadcaster
    m_eventBroadcaster = (EventBroadcaster*)&m_systemContainer->CreateNewSystem("EventBroadcaster");

    //TaskManager
    m_taskManagingSystem = (TaskManager*) &m_systemContainer->CreateNewSystem("TaskManager");

    //Flatbuffers System
    m_fbSystem = (FlatBuffersSystem*)&m_systemContainer->CreateNewSystem("FlatBuffersSystem");

    //Graphics Dx11
    m_graphicsSystem = (XC_Graphics*)&m_systemContainer->CreateNewSystem("GraphicsSystem");
    m_graphicsSystem->InitGraphicsWindow(m_hMainWnd, m_clientWidth, m_clientHeight, true);

    //Init Input Handlers which depend on above window created.
    m_directInputSystem = (DirectInput*)&m_systemContainer->CreateNewSystem("InputSystem");
    m_directInputSystem->Init(GetAppInst(), m_hMainWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    //Resource Manager
    m_resourceManagingSystem = (ResourceManager*) &m_systemContainer->CreateNewSystem("ResourceManager");
    m_resourceManagingSystem->Init(*m_taskManagingSystem);

    //Initialize Cameras
    m_cameraManagingSystem = (XC_CameraManager*)&m_systemContainer->CreateNewSystem("CameraManager");
    m_cameraManagingSystem->InitializeCameras(*m_graphicsSystem, *m_directInputSystem, m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->SetCameraType(CAMERATYPE_BASIC);

    //Gameplay state machine
    m_gameFSM = (GameFiniteStateMachine*) &m_systemContainer->CreateNewSystem("GameFSM");
    m_gameFSM->Init();
    m_gameFSM->SetState("IntroState", STATE_NONE);

    //Network Manger
    m_networkManagingSystem = (NetworkManager*)&m_systemContainer->CreateNewSystem("NetworkManager");

#if defined(LIVE_DRIVE_ENABLED)
    //Initalize the network clients
    m_liveDriveClient = new LiveDriveVRClient();
    m_liveDriveClient->init(IP_ADDRESS, DEFAULT_PORT);

    m_networkManagingSystem->AddNetPeer(m_liveDriveClient);
#endif

    //Show the window.
    InitMainWindow();

    m_appInitialized = true;

    return true;
}

void Game_Win32::RegisterSystems()
{
    m_systemContainer = &SystemLocator::GetInstance()->GetSystemContainer();

    m_systemContainer->RegisterSystem<Timer>("Timer");
    m_systemContainer->RegisterSystem<EventBroadcaster>("EventBroadcaster");
    m_systemContainer->RegisterSystem<DirectInput_Win32>("InputSystem");
    m_systemContainer->RegisterSystem<TaskManager>("TaskManager");

#if defined(XCGRAPHICS_DX12)
    m_systemContainer->RegisterSystem<XC_GraphicsDx12>("GraphicsSystem");
#elif defined(XCGRAPHICS_DX11)
    m_systemContainer->RegisterSystem<XC_GraphicsDx11>("GraphicsSystem");
#elif defined(XCGRAPHICS_GL)
    m_systemContainer->RegisterSystem<XC_GraphicsGL>("GraphicsSystem");
#endif

    m_systemContainer->RegisterSystem<FlatBuffersSystem>("FlatBuffersSystem");
    m_systemContainer->RegisterSystem<ResourceManager>("ResourceManager");
    m_systemContainer->RegisterSystem<XC_CameraManager>("CameraManager");
    m_systemContainer->RegisterSystem<GameFiniteStateMachine>("GameFSM");
    m_systemContainer->RegisterSystem<NetworkManager>("NetworkManager");
}

void Game_Win32::OnResize()
{
    m_graphicsSystem->OnResize(m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->onResize(m_clientWidth, m_clientHeight);
}

void Game_Win32::Update(float dt)
{
    m_timer->Update();
    m_eventBroadcaster->Update();

    m_directInputSystem->Update();

    m_graphicsSystem->Update(dt);
    m_resourceManagingSystem->Update();

    m_networkManagingSystem->Update();

    m_gameFSM->Update(dt);

    //This update is necessary after WORLD is drawn, because of camera dependency on some of the object's of world
    m_cameraManagingSystem->Update(dt);

    //Do the tasks if any
    m_taskManagingSystem->Update();
}

void Game_Win32::Draw()
{
#if defined(LIVE_DRIVE_ENABLED)
    //Live drive capture
    m_graphicsSystem->BeginSecondaryScene();
    m_cameraManagingSystem->Draw();
    m_gameFSM->Draw(*m_graphicsSystem);
    m_graphicsSystem->GetRenderingPool().Render();
    m_graphicsSystem->EndSecondaryScene();

    m_graphicsSystem->GetShaderManagerSystem().ClearShaderAndRenderStates(*m_graphicsSystem->GetDeviceContext());
#endif

    //draw every other object
    m_graphicsSystem->BeginScene();

    m_cameraManagingSystem->Draw();
    m_gameFSM->Draw(*m_graphicsSystem);
    m_graphicsSystem->GetRenderingPool().Render();
    
    m_graphicsSystem->EndScene();

    m_graphicsSystem->GetShaderManagerSystem().ClearShaderAndRenderStates(*m_graphicsSystem->GetDeviceContext());
}

void Game_Win32::EnableFullScreenMode(bool enable)
{
    m_graphicsSystem->GoFullscreen(enable);
}

void Game_Win32::Destroy()
{
    m_gameFSM->Destroy();
    m_cameraManagingSystem->Destroy();
    m_directInputSystem->Destroy();
    m_graphicsSystem->Destroy();
    m_resourceManagingSystem->Destroy();
    m_taskManagingSystem->UnregisterAllTasks();
    m_networkManagingSystem->Destroy();
    m_eventBroadcaster->Destroy();

    SystemLocator::GetInstance()->Destroy();
}