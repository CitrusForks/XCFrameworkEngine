#include "stdafx.h"

#include "Game_Editor.h"
#include "Engine/System/SystemLocator.h"
#include "Engine/Network/Clients/LiveDriveVRClient.h"

void Game_Editor::Destroy()
{
    m_systemContainer.Destroy();

    m_gameFSM->Destroy();
    m_cameraManagingSystem->Destroy();

    m_directInputSystem->Destroy();
    m_XCGraphicsSystem->Destroy();
    m_resourceManagingSystem->Destroy();
    m_taskManagingSystem->UnregisterAllTasks();
    m_networkManagingSystem->Destroy();
}

Game_Editor::Game_Editor(HINSTANCE hInstance, std::string winCaption, HWND hwnd, int width, int height)
    : AppFramework_Editor(hInstance, winCaption, hwnd, width, height)
{
    srand(time_t(0));
    Init();
}

void Game_Editor::Init()
{
    AppFramework_Editor::Init();
    SystemLocator::GetInstance()->RegisterSystemContainer(m_systemContainer);

    //Init Input Handlers
    m_directInputSystem = new DirectInput();
    m_directInputSystem->Init(GetAppInst(), GetMainWnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    m_systemContainer.AddSystem(GetInputSystem());

    HWND window = GetMainWnd();

    m_taskManagingSystem = new TaskManager();
    m_systemContainer.AddSystem(GetTaskManagerSystem());

    m_XCGraphicsSystem = new XC_Graphics();
    m_XCGraphicsSystem->InitGraphicsWindow(window, m_clientWidth, m_clientHeight, true);
    m_systemContainer.AddSystem(GetXCGraphicsSystem());

    m_resourceManagingSystem = new ResourceManager(*m_XCGraphicsSystem, *m_taskManagingSystem);
    m_resourceManagingSystem->Init();
    m_systemContainer.AddSystem(GetResourceManagerSystem());

    //Initialize Cameras
    m_cameraManagingSystem = new XC_CameraManager(m_XCGraphicsSystem->GetShaderManagerSystem(), GetInputSystem());
    m_cameraManagingSystem->InitializeCameras(m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->SetCameraType(CAMERATYPE_BASIC);
    m_systemContainer.AddSystem(GetCameraManagerSystem());

    m_gameFSM = new GameFiniteStateMachine();
    m_gameFSM->Init();
    m_systemContainer.AddSystem(GetGameFSMSystem());

    m_gameFSM->SetState("EditorLoadingState", STATE_NONE);

    m_networkManagingSystem = new NetworkManager();
    m_systemContainer.AddSystem(*m_networkManagingSystem);

    //Initalize the netowrk clients
    m_liveDriveClient = new LiveDriveVRClient(*this);
    m_liveDriveClient->init(IP_ADDRESS, DEFAULT_PORT);

    m_networkManagingSystem->AddNetPeer(m_liveDriveClient);
}

Game_Editor::~Game_Editor()
{
    delete(m_cameraManagingSystem);
    delete(m_gameFSM);

    delete(m_resourceManagingSystem);
    delete(m_XCGraphicsSystem);
    delete(m_taskManagingSystem);
    delete(m_directInputSystem);
    delete(m_networkManagingSystem);
}

void Game_Editor::OnResize()
{
    m_XCGraphicsSystem->OnResize(m_clientWidth, m_clientHeight);
    m_cameraManagingSystem->onResize(m_clientWidth, m_clientHeight);
}

void Game_Editor::EnableFullScreenMode(bool enable)
{
    m_XCGraphicsSystem->GoFullscreen(enable);
}

void Game_Editor::UpdateScene(float dt)
{
    m_directInputSystem->Update();

    m_XCGraphicsSystem->Update(dt);
    m_resourceManagingSystem->Update();

    m_networkManagingSystem->Update();

    m_gameFSM->Update(dt);

    //This update is necessary after WORLD is drawn, because of camera dependency on some of the object's of world
    m_cameraManagingSystem->Update(dt);

    //Do the tasks if any
    m_taskManagingSystem->DoWork();

}

void Game_Editor::DrawScene()
{
    //Live drive capture
    m_XCGraphicsSystem->BeginSecondaryScene();
    m_gameFSM->Draw(*m_XCGraphicsSystem);
    m_XCGraphicsSystem->EndSecondaryScene();

    m_XCGraphicsSystem->BeginScene();
    //draw every other object
    m_gameFSM->Draw(*m_XCGraphicsSystem);
    m_XCGraphicsSystem->EndScene();
}