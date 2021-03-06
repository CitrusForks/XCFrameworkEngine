/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GamePrecompiledHeader.h"

#include "Win32/AppFrameworkWin32.h"
#include "Base/Memory/MemoryOverrides.h"

namespace
{
    static AppFrameworkWin32* g_pD3DApp = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hwnd,u32 msg,WPARAM wParam,LPARAM lParam)
{
    if(g_pD3DApp != 0)
        return g_pD3DApp->MsgProc(hwnd, msg, wParam, lParam);
    else
        return DefWindowProc(hwnd,msg,wParam,lParam);
}

AppFrameworkWin32::AppFrameworkWin32(HINSTANCE hInstance,std::string winCaption, bool enable4xMsaa)
{
    m_appInitialized = false;
    m_MainWndCaption = winCaption;
    m_Enable4xMsaa = enable4xMsaa;
    m_clientWidth = 1024;
    m_clientHeight = 768;

    m_hAppInst = hInstance;
    m_hMainWnd = 0;

    g_pD3DApp = this;
}


AppFrameworkWin32::~AppFrameworkWin32()
{
}

i32 AppFrameworkWin32::Init()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_cntsPerSec);
    m_secsPerCnt = 1.0f / (f32)m_cntsPerSec;
    QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTimeStamp);

    CreateMainWindow();
    return true;
}

void AppFrameworkWin32::Destroy()
{
#if !defined(XCGRAPHICS_GL)
    delete(m_hAppInst);
    delete(m_hMainWnd);
#endif
}

void AppFrameworkWin32::CreateMainWindow()
{
#if defined(XCGRAPHICS_GL)
    i32 ct = 0;
    char* args = "";
    glutInit(&ct, &args);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(m_clientWidth, m_clientHeight);
    glutCreateWindow("XC Framework OpenGL");

    glewExperimental = GL_TRUE;
    if (glewInit())
    {
        XCASSERT(false);
        exit(0);
    }
#else
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hAppInst;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "XC_Framework";

    if (!RegisterClass(&wc))
    {
        MessageBox(0, "Failed to create window", 0, 0);
        PostQuitMessage(0);
    }

    RECT rect = { 0, 0, m_clientWidth, m_clientHeight };

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    m_hMainWnd = ::CreateWindow("XC_Framework", m_MainWndCaption.c_str(),
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, m_clientWidth, m_clientHeight,
                             0, 0, m_hAppInst, 0);

    if (!m_hMainWnd)
    {
        MessageBox(0, "Create Window Failed", 0, 0);
        PostQuitMessage(0);
    }
#endif
}


void AppFrameworkWin32::InitMainWindow()
{
#if defined(XCGRAPHICS_GL)
    glutDisplayFunc(&GLRun);
    glutMainLoop();
#else
    ShowWindow(m_hMainWnd, SW_SHOW);
    UpdateWindow(m_hMainWnd);
#endif
}

void AppFrameworkWin32::Run()
{
#if defined(XCGRAPHICS_GL)
    if (m_appInitialized)
    {
        __int64 currTimeStamp = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

        f32 dt = (currTimeStamp - m_prevTimeStamp) * m_secsPerCnt;

        Update(dt);
        Draw();

        m_prevTimeStamp = currTimeStamp;
    }
#else
    MSG msg = {0};

    while(msg.message!=WM_QUIT)
    {

        if(PeekMessage(&msg,0,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if(m_AppPaused)
            {
                Sleep(20);
                continue;
            }
            else
            {
                __int64 currTimeStamp=0;
                QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

                f32 dt = (currTimeStamp- m_prevTimeStamp) * m_secsPerCnt;

                Update(dt);
                Draw();

                m_prevTimeStamp = currTimeStamp;
            }
        }
    }
#endif
}

LRESULT AppFrameworkWin32::MsgProc(HWND hwnd, u32 msg,WPARAM wParam,LPARAM lParam)
{
    RECT clientRect={0,0,0,0};

    switch(msg)
    {
        //WM_ACTIVE- is sent when windo is activated or deactivated
        //We pause the game when the main window is deactivated and 
        //unpause when it becomes active
    case WM_ACTIVATE:
                    if(LOWORD(wParam) == WA_INACTIVE)
                        m_AppPaused = true;
                    else
                        m_AppPaused = false;
                    return 0;

// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    // Here we reset everything based on the new window dimensions.
    case WM_ENTERSIZEMOVE:
                    m_AppPaused = true;

                    return 0;
    
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
                    GetClientRect(m_hMainWnd,&clientRect);
                    m_clientWidth = clientRect.right;
                    m_clientHeight = clientRect.bottom;
                    m_AppPaused = false;
                    
                    //TODO : WIP. Crashes on dx12
                    //OnResize();
                    
                    return 0;

    case WM_CLOSE:
         DestroyWindow(m_hMainWnd);
         return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            EnableFullScreenMode(false);
        }
        else if (wParam == 'F')
        {
            EnableFullScreenMode(true);
        }

        return 0;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

void AppFrameworkWin32::OnResize()
{
}

bool AppFrameworkWin32::IsDeviceLost()
{
     return false;
}

void AppFrameworkWin32::GLRun()
{
    g_pD3DApp->Run();
}