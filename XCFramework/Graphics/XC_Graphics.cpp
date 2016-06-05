/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XC_Graphics.h"

XC_Graphics::XC_Graphics(void)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_initDone = false;
}

XC_Graphics::~XC_Graphics(void)
{
}

void XC_Graphics::Destroy()
{
    m_XCShaderSystem->Destroy();
    m_renderingPool->Destroy();
}

void XC_Graphics::InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa)
{
    m_hMainWnd = _mainWnd;
    m_ClientWidth = _width;
    m_ClientHeight = _height;
    m_Enable4xMsaa = _enable4xMsaa;
}

void XC_Graphics::SetupPipeline()
{
}

void XC_Graphics::SetupShadersAndRenderPool()
{
    //Initialize Shader System
    m_XCShaderSystem = new XC_ShaderManager(*m_pD3DDevice);
    m_XCShaderSystem->Init();

    //Initialize the rendering pool
    m_renderingPool = new RenderingPool();
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
    m_ScreenViewPort[RENDERTARGET_MAIN_0].Width = (float)m_ClientWidth;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].Height = (float)m_ClientHeight;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_MAIN_0].MaxDepth = 1.0f;

    //Set the Viewport Live Drive
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].TopLeftX = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].TopLeftY = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Width = (float)256;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Height = (float)256;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].MinDepth = 0.0f;
    m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].MaxDepth = 1.0f;

    //And its scissor
    m_scissorRect.right = static_cast<LONG>(m_ClientWidth);
    m_scissorRect.bottom = static_cast<LONG>(m_ClientHeight);
#endif
}

void XC_Graphics::Update(float dt)
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

void XC_Graphics::OnResize(int _width, int _height)
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