#include "stdafx.h"

#include "Engine/ApplicationFramework/Editor/AppFramework_Editor.h"
#include "Game/Game _Editor.h"


#if defined(WIN_32)

#if defined(EDITOR)
extern "C" __declspec(dllexport) void Launch_AppFramework_Editor(HINSTANCE hInstance, HWND hwnd, int width, int height);

void AppFramework_EditorImpl(void* param)
{
    Game* app = (Game*)param;
    app->Run();
    app->Destroy();

    delete(app);
}


void Launch_AppFramework_Editor(HINSTANCE hInstance, HWND hWnd, int width, int height)
{
    Game *app = new Game(hInstance, "XC Framework Editor", hWnd, width, height);
    unsigned long handle = 0;
    HANDLE threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&AppFramework_EditorImpl, app, 0, &handle);
}
#endif

#endif


AppFramework_Editor::AppFramework_Editor(HINSTANCE hInstance, std::string wndCaption, HWND hwnd, int width, int height)
{
    m_MainWndCaption = wndCaption;
    m_Enable4xMsaa = true;
    m_clientWidth = width;
    m_clientHeight = height;

    m_hAppInst = hInstance;
    m_hMainWnd = hwnd;
}

AppFramework_Editor::~AppFramework_Editor()
{

}

void AppFramework_Editor::Init()
{

}

void AppFramework_Editor::Destroy()
{
    delete(m_hMainWnd);
    delete(m_hAppInst);
}

HWND AppFramework_Editor::CreateD3DChildWindow()
{
    m_hMainWnd = CreateWindowEx(0, m_MainWndCaption.c_str(), "Editor",
                   WS_CHILD | WS_VISIBLE,
                   CW_USEDEFAULT, CW_USEDEFAULT,
                   m_clientWidth, m_clientHeight,
                   m_hMainWnd,
                   0,
                   m_hAppInst,
                   0);

    if (!m_hMainWnd)
    {
        MessageBox(0, "Create Window Failed", 0, 0);
        PostQuitMessage(0);
    }

    return m_hMainWnd;
}

void AppFramework_Editor::InitMainWindow()
{
	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);
}


int AppFramework_Editor::Run()
{
    MSG msg = {0};
    
    __int64 cntsPerSec = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*) &cntsPerSec);
    float secsPerCnt= 1.0f / (float)cntsPerSec;
    
    __int64 prevTimeStamp=0;
    QueryPerformanceCounter((LARGE_INTEGER*) &prevTimeStamp);
    
    while(msg.message!=WM_QUIT)
    {
        
        if(PeekMessage(&msg,0,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //otherwise do animation/game stuffs
        else
        {
            __int64 currTimeStamp=0;
            QueryPerformanceCounter((LARGE_INTEGER*) &currTimeStamp);
            
            float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
            
            UpdateScene(dt);
            DrawScene();
            
            prevTimeStamp = currTimeStamp;
        }
    }
    return (int)msg.wParam;
}

void AppFramework_Editor::OnResize()
{
	
}

void AppFramework_Editor::EnableFullScreenMode(bool enable)
{
	
}


bool AppFramework_Editor::IsDeviceLost()
{
	return false;
}

