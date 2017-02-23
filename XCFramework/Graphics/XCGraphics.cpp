/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XCGraphics.h"
#include "GPUResourceSystem.h"

XCGraphics::XCGraphics(void)
    : m_pD3DDevice(nullptr)
    , m_pD3DDeviceContext(nullptr)
    , m_xcShaderSystem(nullptr)
    , m_renderingPool(nullptr)
    , m_sharedDescriptorHeap(nullptr)
    , m_gpuResourceSystem(nullptr)
    , m_secondaryDrawCall(false)
    , m_frameIndex(0)
    , m_4xMsaaQuality(false)
    , m_enable4xMsaa(false)
    , m_clientWidth(WindowWidth)
    , m_clientHeight(WindowHeight)
    , m_initDone(false)
{
    ZeroMemory(&m_ScreenViewPort, sizeof(D3D_VIEWPORT));
}

XCGraphics::~XCGraphics(void)
{
}

void XCGraphics::Destroy()
{
    if (m_xcShaderSystem)
    {
        m_xcShaderSystem->Destroy();
        XCDELETE(m_xcShaderSystem);
    }

    if (m_renderingPool)
    {
        m_renderingPool->Destroy();
        XCDELETE(m_renderingPool);
    }

    for (u32 rIndex = 0; rIndex < RenderTargetType_Max; ++rIndex)
    {
        if (m_renderTargets[rIndex])
        {
            m_renderTargets[rIndex]->Destroy();
            XCDELETE(m_renderTargets[rIndex]);
        }

        if (m_depthStencilResource[rIndex])
        {
            m_gpuResourceSystem->DestroyResource(m_depthStencilResource[rIndex]);
        }
    }

    ReleaseCOM(m_pD3DDevice);
}

void XCGraphics::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    m_hMainWnd = _mainWnd;
    m_clientWidth = _width;
    m_clientHeight = _height;
    m_enable4xMsaa = _enable4xMsaa;

    ISystem::Init();
}

ID3DDevice * XCGraphics::GetDevice()
{
    return m_pD3DDevice;
}

void XCGraphics::SetupPipeline()
{
}

void XCGraphics::SetupShadersAndRenderPool()
{
    //Initialize Shader System
    m_xcShaderSystem = XCNEW(XCShaderContainer)(*m_pD3DDevice);
    m_xcShaderSystem->Init();

    //Initialize the rendering pool
    m_renderingPool = XCNEW(RenderingPool)();
    m_renderingPool->Init();
}

void XCGraphics::OnRenderComplete()
{
    m_renderingPool->OnRenderComplete();
}

void XCGraphics::SetupDevice()
{
}

void XCGraphics::SetupSwapChain()
{
}

void XCGraphics::SetupRenderTargets()
{
}

void XCGraphics::SetupDepthStencilBuffer()
{
}

void XCGraphics::SetupDepthStencilStates()
{
}

void XCGraphics::SetupDepthView()
{
}

void XCGraphics::SetupViewPort()
{
#if defined(WIN32)
    //Set the Viewport
    m_ScreenViewPort[RenderTargetType_Main_0].TopLeftX  = 0.0f;
    m_ScreenViewPort[RenderTargetType_Main_0].TopLeftY  = 0.0f;
    m_ScreenViewPort[RenderTargetType_Main_0].Width     = f32(m_clientWidth);
    m_ScreenViewPort[RenderTargetType_Main_0].Height    = f32(m_clientHeight);
    m_ScreenViewPort[RenderTargetType_Main_0].MinDepth  = 0.0f;
    m_ScreenViewPort[RenderTargetType_Main_0].MaxDepth  = 1.0f;

    m_ScreenViewPort[RenderTargetType_Main_1]           = m_ScreenViewPort[RenderTargetType_Main_0];

    m_ScreenViewPort[RenderTargetType_GBuffer_Diffuse]  = m_ScreenViewPort[RenderTargetType_Main_0];

    m_ScreenViewPort[RenderTargetType_GBuffer_Position] = m_ScreenViewPort[RenderTargetType_Main_0];

    m_ScreenViewPort[RenderTargetType_GBuffer_Normal]   = m_ScreenViewPort[RenderTargetType_Main_0];

    m_ScreenViewPort[RenderTargetType_Debug]            = m_ScreenViewPort[RenderTargetType_Main_0];

    //Set the Viewport Live Drive
    m_ScreenViewPort[RenderTargetType_LiveDrive]        = m_ScreenViewPort[RenderTargetType_Main_0];
    m_ScreenViewPort[RenderTargetType_LiveDrive].Width  = f32(256);
    m_ScreenViewPort[RenderTargetType_LiveDrive].Height = f32(256);

    //And its scissor
    m_scissorRect.right = static_cast<LONG>(m_clientWidth);
    m_scissorRect.bottom = static_cast<LONG>(m_clientHeight);
#endif
}

void XCGraphics::Update(f32 dt)
{
}

void XCGraphics::BeginScene()
{
    //On immediate context, we only want to work on current frame render target, so set and clear
    std::vector<RenderTargetsType> rtvs = { (RenderTargetsType) m_frameIndex };

    SetRenderableTargets(*m_pD3DDeviceContext, rtvs);
    ClearRTVAndDSVs(*m_pD3DDeviceContext, rtvs, XCVec4(1.0f, 1.0f, 1.0f, 1.0f));

    //On deferred context, we work on multiple render targets for deferred lighting. So work on them.
    rtvs.push_back(RenderTargetType_GBuffer_Diffuse);
    rtvs.push_back(RenderTargetType_GBuffer_Position);
    rtvs.push_back(RenderTargetType_GBuffer_Normal);

    m_renderingPool->Begin(rtvs);
}

void XCGraphics::Render()
{
    m_renderingPool->Render();
}

void XCGraphics::BeginSecondaryScene()
{
}

void XCGraphics::EndSecondaryScene()
{
}

void XCGraphics::EndScene()
{
    m_renderingPool->End();
}

void XCGraphics::TurnOnZ()
{
}

void XCGraphics::TurnOffZ()
{
}

void XCGraphics::SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn)
{
}

void XCGraphics::GoFullscreen(bool go)
{
}

void XCGraphics::OnResize(i32 _width, i32 _height)
{
}

std::string XCGraphics::GetDefaultWindowTitle()
{
#if defined(XCGRAPHICS_DX11)
    return "XCFramework DirectX 11";
#elif defined(XCGRAPHICS_DX12)
    return "XCFramework DirectX 12";
#elif defined(XCGRAPHICS_GNM)
    return "XCFramework OpenGL";
#endif

}

void XCGraphics::SetWindowTitle(std::string value)
{
    SetWindowText(m_hMainWnd, value.c_str());
}