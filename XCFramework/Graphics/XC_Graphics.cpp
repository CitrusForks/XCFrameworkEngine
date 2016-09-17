/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XC_Graphics.h"
#include "GPUResourceSystem.h"

XC_Graphics::XC_Graphics(void)
    : m_pD3DDevice(nullptr)
    , m_XCShaderSystem(nullptr)
    , m_renderingPool(nullptr)
    , m_secondaryDrawCall(false)
    , m_4xMsaaQuality(false)
    , m_Enable4xMsaa(false)
    , m_ClientWidth(1024)
    , m_ClientHeight(768)
    , m_initDone(false)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ZeroMemory(&m_ScreenViewPort, sizeof(D3D_VIEWPORT));
}

XC_Graphics::~XC_Graphics(void)
{
}

void XC_Graphics::Destroy()
{
    if (m_XCShaderSystem)
    {
        m_XCShaderSystem->Destroy();
        XCDELETE(m_XCShaderSystem);
    }

    if (m_renderingPool)
    {
        m_renderingPool->Destroy();
        XCDELETE(m_renderingPool);
    }

    for (u32 rIndex = 0; rIndex < RENDERTARGET_MAX; ++rIndex)
    {
        if (m_renderTargets[rIndex])
        {
            m_renderTargets[rIndex]->Destroy();
            XCDELETE(m_renderTargets[rIndex]);
        }
    }

    ReleaseCOM(m_pD3DDevice);
}

void XC_Graphics::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    m_hMainWnd = _mainWnd;
    m_ClientWidth = _width;
    m_ClientHeight = _height;
    m_Enable4xMsaa = _enable4xMsaa;

    ISystem::Init();
}

void XC_Graphics::SetupPipeline()
{
}

void XC_Graphics::SetupShadersAndRenderPool()
{
    //Initialize Shader System
    m_XCShaderSystem = XCNEW(XC_ShaderContainer)(*m_pD3DDevice);
    m_XCShaderSystem->Init();

    //Initialize the rendering pool
    m_renderingPool = XCNEW(RenderingPool)();
    m_renderingPool->Init();
}

void XC_Graphics::SetupDevice()
{
}

void XC_Graphics::SetupSwapChain()
{
}

void XC_Graphics::SetupRenderTargets()
{
}

void XC_Graphics::SetupDepthStencilBuffer()
{
}

void XC_Graphics::SetupDepthStencilStates()
{
}

void XC_Graphics::SetupDepthView()
{
}

void XC_Graphics::SetupViewPort()
{
#if defined(WIN32)
    //Set the Viewport
    m_ScreenViewPort[RENDERTARGET_MAIN_0].TopLeftX = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].TopLeftY = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].Width = (f32)m_ClientWidth;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].Height = (f32)m_ClientHeight;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].MaxDepth = 1.0f;

    //Set the Viewport
    m_ScreenViewPort[RENDERTARGET_MAIN_1].TopLeftX = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_1].TopLeftY = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_1].Width = (f32)m_ClientWidth;
    m_ScreenViewPort[RENDERTARGET_MAIN_1].Height = (f32)m_ClientHeight;
    m_ScreenViewPort[RENDERTARGET_MAIN_1].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_1].MaxDepth = 1.0f;

    //Set the Viewport
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].TopLeftX = 0.0f;
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].TopLeftY = 0.0f;
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].Width = (f32)m_ClientWidth;
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].Height = (f32)m_ClientHeight;
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].MaxDepth = 1.0f;

    //Set the Viewport Live Drive
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].TopLeftX = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].TopLeftY = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Width = (f32)256;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Height = (f32)256;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].MaxDepth = 1.0f;

    //And its scissor
    m_scissorRect.right = static_cast<LONG>(m_ClientWidth);
    m_scissorRect.bottom = static_cast<LONG>(m_ClientHeight);
#endif
}

void XC_Graphics::Update(f32 dt)
{
}

void XC_Graphics::BeginScene()
{
}

void XC_Graphics::BeginSecondaryScene()
{
}

void XC_Graphics::EndSecondaryScene()
{
}

void XC_Graphics::EndScene()
{
}

void XC_Graphics::TurnOnZ()
{
}

void XC_Graphics::TurnOffZ()
{
}

void XC_Graphics::SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn)
{
}

void XC_Graphics::GoFullscreen(bool go)
{
}

void XC_Graphics::OnResize(i32 _width, i32 _height)
{
}

std::string XC_Graphics::GetDefaultWindowTitle()
{
#if defined(XCGRAPHICS_DX11)
    return "XCFramework DirectX 11";
#elif defined(XCGRAPHICS_DX12)
    return "XCFramework DirectX 12";
#elif defined(XCGRAPHICS_GNM)
    return "XCFramework OpenGL";
#endif

}

void XC_Graphics::SetWindowTitle(std::string value)
{
    SetWindowText(m_hMainWnd, value.c_str());
}