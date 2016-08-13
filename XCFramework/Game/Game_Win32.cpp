/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GamePrecompiledHeader.h"

#include "Game_Win32.h"

#include "Assets/Packages/PackageConsts.h"

#include "Engine/System/SystemLocator.h"
#include "Engine/Memory/MemoryOverrides.h"
#include "Engine/Memory/MemorySystem.h"
#include "Engine/Memory/MemorySystemWin32.h"
#include "Engine/Event/EventBroadcaster.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Input/Directinput.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Input/Directinput_Win32.h"

#include "Graphics/XC_Graphics.h"
#if defined(XCGRAPHICS_DX12)
#include "Graphics/XC_GraphicsDx12.h"
#elif defined(XCGRAPHICS_DX11)
#include "Graphics/XC_GraphicsDx11.h"
#elif defined(XCGRAPHICS_GL)
#include "Graphics/XC_GraphicsGL.h"
#endif

#include "Gameplay/XC_Camera/XC_CameraManager.h"
#include "Gameplay/GameFiniteStateMachine.h"

#include "Network/NetworkManager.h"
#include "Network/INetPeer.h"
#include "Network/Clients/LiveDriveVRClient.h"


Game_Win32::Game_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa)
    : AppFramework_Win32(hInstance, winCaption, enable4xMsaa)
    , m_systemContainer(nullptr)
    , m_memorySystem(nullptr)
    , m_eventBroadcaster(nullptr)
    , m_taskManagingSystem(nullptr)
    , m_fbSystem(nullptr)
    , m_graphicsSystem(nullptr)
    , m_directInputSystem(nullptr)
    , m_resourceManagingSystem(nullptr)
    , m_cameraManagingSystem(nullptr)
    , m_gameFSM(nullptr)
    , m_networkManagingSystem(nullptr)
    , m_liveDriveClient(nullptr)
{
    srand(time_t(0));
    Logger("[GAME MAIN]");
}

Game_Win32::~Game_Win32()
{
}

i32 Game_Win32::Init()
{
    AppFramework_Win32::Init();

    //Register Systems
    RegisterSystems();

    //Initialize memory System
    m_memorySystem = (MemorySystem*)&m_systemContainer->CreateNewSystem("MemorySystem");
    m_memorySystem->Init(1024 * 1024 * 1024);

    //Event Broadcaster
    m_eventBroadcaster = (EventBroadcaster*)&m_systemContainer->CreateNewSystem("EventBroadcaster");
    m_eventBroadcaster->Init();

    //TaskManager
    m_taskManagingSystem = (TaskManager*) &m_systemContainer->CreateNewSystem("TaskManager");
    m_taskManagingSystem->Init();

    //Flatbuffers System
    m_fbSystem = (FlatBuffersSystem*)&m_systemContainer->CreateNewSystem("FlatBuffersSystem");
    m_fbSystem->Init();

    //Graphics Dx11
    m_graphicsSystem = (XC_Graphics*)&m_systemContainer->CreateNewSystem("GraphicsSystem");
    m_graphicsSystem->Init(m_hMainWnd, m_clientWidth, m_clientHeight, true);

    //Init Input Handlers which depend on above window created.
    m_directInputSystem = (DirectInput*)&m_systemContainer->CreateNewSystem("InputSystem");
    m_directInputSystem->Init(GetAppInst(), m_hMainWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    //Resource Manager
    m_resourceManagingSystem = (ResourceManager*) &m_systemContainer->CreateNewSystem("ResourceManager");
    m_resourceManagingSystem->Init(*m_taskManagingSystem);

    //Initialize Cameras
    m_cameraManagingSystem = (XC_CameraManager*)&m_systemContainer->CreateNewSystem("CameraManager");
    m_cameraManagingSystem->Init(*m_graphicsSystem, m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->SetCameraType(CAMERATYPE_BASIC);

    //Gameplay state machine
    m_gameFSM = (GameFiniteStateMachine*) &m_systemContainer->CreateNewSystem("GameFSM");
    m_gameFSM->Init();
    m_gameFSM->SetState("IntroState", STATE_NONE);

    //Network Manger
    m_networkManagingSystem = (NetworkManager*)&m_systemContainer->CreateNewSystem("NetworkManager");
    m_networkManagingSystem->Init();

#if defined(LIVE_DRIVE_ENABLED)
    //Initalize the network clients
    m_liveDriveClient = XCNEW(LiveDriveVRClient);
    m_liveDriveClient->Init(IP_ADDRESS, DEFAULT_PORT);

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

    m_systemContainer->RegisterSystem<MemorySystemWin32>("MemorySystem");
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

void Game_Win32::Update(f32 dt)
{
    m_memorySystem->Update(dt);

    m_eventBroadcaster->Update();

    m_directInputSystem->Update();

    m_resourceManagingSystem->Update();

    m_cameraManagingSystem->Update(dt);

    m_gameFSM->Update(dt);

    m_taskManagingSystem->Update();

    m_graphicsSystem->Update(dt);

    m_networkManagingSystem->Update();
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
    m_taskManagingSystem->Destroy();
    m_networkManagingSystem->Destroy();
    m_eventBroadcaster->Destroy();
    m_memorySystem->Destroy();

    SystemLocator::GetInstance()->Destroy();
}


i32 WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in i32 nShowCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    Game_Win32 app(hInstance, "", true);
    app.Init();
    app.Run();
    app.Destroy();

    return 0;
}