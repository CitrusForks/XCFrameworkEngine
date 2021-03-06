/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GamePrecompiledHeader.h"

#include "GameWin32.h"

#include "Assets/Packages/PackageConsts.h"

#include "Base/Memory/MemoryOverrides.h"
#include "Base/Memory/MemorySystem.h"
#include "Base/Memory/MemorySystemWin32.h"
#include "Base/Serializer/BaseIDGenerator.h"

#include "Engine/Event/EventBroadcaster.h"
#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Input/XCInput.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Input/XCInputWin32.h"
#include "Engine/DebugInterface/DebugInterface.h"

#include "Graphics/XCGraphics.h"
#if defined(XCGRAPHICS_DX12)
#include "Graphics/XCGraphicsDx12.h"
#elif defined(XCGRAPHICS_DX11)
#include "Graphics/XCGraphicsDx11.h"
#elif defined(XCGRAPHICS_GL)
#include "Graphics/XCGraphicsGL.h"
#endif
#include "Graphics/XCLighting/XCLightManager.h"

#include "Gameplay/XCCamera/XCCameraManager.h"
#include "Gameplay/GameFiniteStateMachine.h"

#include "Network/NetworkManager.h"
#include "Network/INetPeer.h"
#include "Network/Clients/LiveDriveVRClient.h"

#include "Physics/PhysicsPlayground.h"

GameWin32::GameWin32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa)
    : AppFrameworkWin32(hInstance, winCaption, enable4xMsaa)
    , m_systemContainer(nullptr)
    , m_memorySystem(nullptr)
    , m_eventBroadcaster(nullptr)
    , m_taskManagingSystem(nullptr)
    , m_fbSystem(nullptr)
    , m_graphicsSystem(nullptr)
    , m_directInputSystem(nullptr)
    , m_resourceManagingSystem(nullptr)
    , m_cameraManagingSystem(nullptr)
    , m_lightsSystem(nullptr)
    , m_gameFSM(nullptr)
    , m_networkManagingSystem(nullptr)
    , m_liveDriveClient(nullptr)
    , m_debugInterface(nullptr)
    , m_baseIDGenerator(nullptr)
{
    srand(time_t(0));
    Logger("[GAME MAIN]");
}

GameWin32::~GameWin32()
{
}

i32 GameWin32::Init()
{
    AppFrameworkWin32::Init();

    //Register Systems
    RegisterSystems();

    //Initialize memory System
    m_memorySystem = (MemorySystem*)&m_systemContainer->CreateNewSystem("MemorySystem");
    m_memorySystem->Init(MemoryHeapSize);

    //Initialize Base Object Id generator
    m_baseIDGenerator = (BaseIDGenerator*) &m_systemContainer->CreateNewSystem("BaseIDGenerator");
    m_baseIDGenerator->Init();

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
    m_graphicsSystem = (XCGraphics*)&m_systemContainer->CreateNewSystem("GraphicsSystem");
    m_graphicsSystem->Init(m_hMainWnd, m_clientWidth, m_clientHeight, false);

    //Init Input Handlers which depend on above window created.
    m_directInputSystem = (XCInput*)&m_systemContainer->CreateNewSystem("InputSystem");
    m_directInputSystem->Init(GetAppInst(), m_hMainWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    //Resource Manager
    m_resourceManagingSystem = (ResourceManager*) &m_systemContainer->CreateNewSystem("ResourceManager");
    m_resourceManagingSystem->Init(*m_taskManagingSystem);

    //Initialize Cameras
    m_cameraManagingSystem = (XCCameraManager*)&m_systemContainer->CreateNewSystem("CameraManager");
    m_cameraManagingSystem->Init(*m_graphicsSystem, m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->SetCameraType(CAMERATYPE_BASIC);

    //Initialize lights
    m_lightsSystem = (XCLightManager*) &m_systemContainer->CreateNewSystem("LightsManager");
    m_lightsSystem->InitializeLights();

    //Gameplay state machine
    m_gameFSM = (GameFiniteStateMachine*) &m_systemContainer->CreateNewSystem("GameFSM");
    m_gameFSM->Init();
    m_gameFSM->SetState("IntroState", STATE_NONE);

    //PhysicsPlayground
    m_physicsPlayground = (PhysicsPlayground*) &m_systemContainer->CreateNewSystem("PhysicsPlayground");
    m_physicsPlayground->Init(PhysicsPlayground::PhysicsPlaygroundDesc(true, true, XCVec4(0.0f, -9.4f, 0.0f, 0.0f)), *m_taskManagingSystem);

    //Network Manger
    m_networkManagingSystem = (NetworkManager*)&m_systemContainer->CreateNewSystem("NetworkManager");
    m_networkManagingSystem->Init();

    //Debug Interface
    m_debugInterface = (DebugInterface*) &m_systemContainer->CreateNewSystem("DebugInterface");
    m_debugInterface->Init();

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

void GameWin32::RegisterSystems()
{
    m_systemContainer = &SystemLocator::GetInstance()->GetSystemContainer();

    m_systemContainer->RegisterSystem<MemorySystemWin32>("MemorySystem");
    m_systemContainer->RegisterSystem<BaseIDGenerator>("BaseIDGenerator");
    m_systemContainer->RegisterSystem<EventBroadcaster>("EventBroadcaster");
    m_systemContainer->RegisterSystem<XCInputWin32>("InputSystem");
    m_systemContainer->RegisterSystem<TaskManager>("TaskManager");

#if defined(XCGRAPHICS_DX12)
    m_systemContainer->RegisterSystem<XCGraphicsDx12>("GraphicsSystem");
#elif defined(XCGRAPHICS_DX11)
    m_systemContainer->RegisterSystem<XCGraphicsDx11>("GraphicsSystem");
#elif defined(XCGRAPHICS_GL)
    m_systemContainer->RegisterSystem<XCGraphicsGL>("GraphicsSystem");
#endif

    m_systemContainer->RegisterSystem<FlatBuffersSystem>("FlatBuffersSystem");
    m_systemContainer->RegisterSystem<ResourceManager>("ResourceManager");
    m_systemContainer->RegisterSystem<XCCameraManager>("CameraManager");
    m_systemContainer->RegisterSystem<XCLightManager>("LightsManager");
    m_systemContainer->RegisterSystem<GameFiniteStateMachine>("GameFSM");
    m_systemContainer->RegisterSystem<NetworkManager>("NetworkManager");
    m_systemContainer->RegisterSystem<DebugInterface>("DebugInterface");
}

void GameWin32::OnResize()
{
    m_graphicsSystem->OnResize(m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->onResize(m_clientWidth, m_clientHeight);
}

void GameWin32::Update(f32 dt)
{
    m_memorySystem->Update(dt);

    m_eventBroadcaster->Update();

    m_directInputSystem->Update();

    m_resourceManagingSystem->Update();

    m_cameraManagingSystem->Update(dt);

    m_lightsSystem->Update(dt);

    m_gameFSM->Update(dt);

    m_debugInterface->Update(dt);

    m_taskManagingSystem->Update();

    m_graphicsSystem->Update(dt);

    m_networkManagingSystem->Update();
}

void GameWin32::Draw()
{
#if defined(LIVE_DRIVE_ENABLED)
    //Live drive capture
    m_graphicsSystem->BeginSecondaryScene();
    m_cameraManagingSystem->Draw();
    m_gameFSM->Draw(*m_graphicsSystem);
    m_graphicsSystem->GetRenderingPool().Render();
    m_graphicsSystem->EndSecondaryScene();

    m_graphicsSystem->GetShaderContainer().ClearShaderAndRenderStates(*m_graphicsSystem->GetDeviceContext());
#endif

    //Begin Draw
    m_graphicsSystem->BeginScene();

    //Draw engine/game specifics
    m_lightsSystem->Draw(*m_graphicsSystem);
    m_cameraManagingSystem->Draw();
    m_gameFSM->Draw(*m_graphicsSystem);

    m_graphicsSystem->Render();
    m_graphicsSystem->EndScene();
    //End Draw

    m_graphicsSystem->GetShaderContainer().ClearShaderAndRenderStates(*m_graphicsSystem->GetDeviceContext());
}

void GameWin32::EnableFullScreenMode(bool enable)
{
    m_graphicsSystem->GoFullscreen(enable);
}

void GameWin32::Destroy()
{
#if defined(LIVE_DRIVE_ENABLED)
    XCDELETE(m_liveDriveClient);
#endif

    m_systemContainer->RemoveSystem("NetworkManager");
    m_systemContainer->RemoveSystem("GameFSM");
    m_systemContainer->RemoveSystem("LightsManager");
    m_systemContainer->RemoveSystem("CameraManager");
    m_systemContainer->RemoveSystem("ResourceManager");
    m_systemContainer->RemoveSystem("FlatBuffersSystem");
    m_systemContainer->RemoveSystem("GraphicsSystem");
    m_systemContainer->RemoveSystem("TaskManager");
    m_systemContainer->RemoveSystem("InputSystem");
    m_systemContainer->RemoveSystem("EventBroadcaster");
    m_systemContainer->RemoveSystem("BaseIDGenerator");
    m_systemContainer->RemoveSystem("MemorySystem");

    SystemLocator::GetInstance()->Destroy();
}


i32 WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in i32 nShowCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    GameWin32 app(hInstance, "", true);
    app.Init();
    app.Run();
    app.Destroy();

    return 0;
}