#pragma once

#include "Engine/ApplicationFramework/Editor/AppFramework_Editor.h"

#include "Engine/TaskManager/TaskManager.h"
#include "Engine/Input/Directinput.h"

#include "Engine/Graphics/XC_GraphicsDx11.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

#include "Engine/Resource/ResourceManager.h"

#include "Engine/Network/INetPeer.h"
#include "Engine/Network/NetworkManager.h"

#include "Gameplay/GameFiniteStateMachine.h"

class Game_Editor : public AppFramework_Editor
{
public:
        Game_Editor(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
        Game_Editor(HINSTANCE hInstance, std::string winCaption, HWND hWnd, int width, int height);
        ~Game_Editor();

        void                    Init();
        void                    OnResize();
        void                    UpdateScene(float dt);
        void                    DrawScene();
        void                    EnableFullScreenMode(bool enable);
        void                    Destroy();

        //System retrival
        DirectInput&            GetInputSystem() { return *m_directInputSystem; }
        TaskManager&            GetTaskManagerSystem() { return *m_taskManagingSystem; }
        XC_Graphics&            GetXCGraphicsSystem() { return *m_XCGraphicsSystem; }
        ResourceManager&        GetResourceManagerSystem() { return *m_resourceManagingSystem; }
        XC_CameraManager&       GetCameraManagerSystem() { return *m_cameraManagingSystem; }
        GameFiniteStateMachine& GetGameFSMSystem() { return *m_gameFSM; }

private:
    INetPeer*                    m_liveDriveClient;

    DirectInput*                 m_directInputSystem;
    XC_Graphics*                 m_XCGraphicsSystem;
    ResourceManager*             m_resourceManagingSystem;
    TaskManager*                 m_taskManagingSystem;
    XC_CameraManager*            m_cameraManagingSystem;
    GameFiniteStateMachine*      m_gameFSM;
    NetworkManager*              m_networkManagingSystem;

    SystemContainer              m_systemContainer;
};