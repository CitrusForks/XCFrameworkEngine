/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_DX11)

#include "XC_GraphicsDx11.h"

#include "Graphics/RenderingPool.h"
#include "Graphics/XC_Shaders/XC_VertexShaderLayout.h"
#include "Graphics/XC_Textures/RenderableTexture.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Graphics/GPUResourceSystem.h"
#include "Graphics/SharedDescriptorHeap.h"

XC_GraphicsDx11::XC_GraphicsDx11(void)
    : m_pSwapChain(nullptr)
    , m_pdxgiFactory(nullptr)
    , m_pD3DDeviceContext(nullptr)
    , m_pRenderTargetView(nullptr)
    , m_renderQuadVB(nullptr)
    , m_renderQuadIB(nullptr)
    , m_depthStencilState(nullptr)
    , m_depthDisabledStencilState(nullptr)
    , m_depthStencilLessEqualState(nullptr)
{
}

XC_GraphicsDx11::~XC_GraphicsDx11(void)
{
}

void XC_GraphicsDx11::Destroy()
{
    XC_Graphics::Destroy();

    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("SharedDescriptorHeap");
    container.RemoveSystem("GPUResourceSystem");

    XCDELETE(m_sharedDescriptorHeap);
    XCDELETE(m_gpuResourceSystem);

    ReleaseCOM(m_pD3DDeviceContext);
    ReleaseCOM(m_pD3DDevice);
    ReleaseCOM(m_pdxgiFactory);
    ReleaseCOM(m_pSwapChain);
    ReleaseCOM(m_pRenderTargetView);
    ReleaseCOM(m_depthDisabledStencilState);
}

void XC_GraphicsDx11::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    XC_Graphics::Init(_mainWnd, _width, _height, _enable4xMsaa);

    SetupPipeline();
    m_initDone = true;
}

void XC_GraphicsDx11::SetupPipeline()
{
    SetupDevice();
    SetupSwapChain();

    CreateDescriptorHeaps();
    CreateGPUResourceSystem();

    SetupViewPort();
    SetupRenderTargets();
    SetupDepthView();
    SetupDepthStencilStates();

    SetupShadersAndRenderPool();

    SetupRenderQuad();
}

void XC_GraphicsDx11::CreateDescriptorHeaps()
{
    //Initialize shader shared descriptor heap
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_sharedDescriptorHeap = (SharedDescriptorHeap*)&container.CreateNewSystem("SharedDescriptorHeap");

    m_sharedDescriptorHeap->Init(*m_pD3DDevice
        , RENDERTARGET_MAX + 1 //No of RTV's
        , RENDERTARGET_MAX + 1 //No of DSV's
        , 1 //No of Samplers
        , 100 //No of CBV, UAV combined
    );
}

void XC_GraphicsDx11::CreateGPUResourceSystem()
{
    //GPU Resource system
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<GPUResourceSystem>("GPUResourceSystem");
    m_gpuResourceSystem = (GPUResourceSystem*) &container.CreateNewSystem("GPUResourceSystem");
    m_gpuResourceSystem->Init(*m_pD3DDevice);
}

void XC_GraphicsDx11::SetupDevice()
{
    u32 createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_DRIVER_TYPE driverType;

#if defined(USE_WRAP_MODE)
    driverType = D3D_DRIVER_TYPE_WARP;
#else
    driverType = D3D_DRIVER_TYPE_HARDWARE;
#endif

#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevel, ARRAYSIZE(featureLevel), D3D11_SDK_VERSION, &m_pD3DDevice, &m_featureLevel, &m_pD3DDeviceContext);

    if (FAILED(hr))
    {
        MessageBox(0, "Direct 3D 11 Create Failed", 0, 0);
        PostQuitMessage(0);
    }
}

void XC_GraphicsDx11::SetupSwapChain()
{
    //Check for Multi Sampling quality
    ValidateResult(m_pD3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));

    //Describing Swap Chains
    m_SwapChainDesc.BufferDesc.Width = m_ClientWidth;
    m_SwapChainDesc.BufferDesc.Height = m_ClientHeight;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.BufferCount = 1;
    if (m_Enable4xMsaa && m_4xMsaaQuality)
    {
        m_SwapChainDesc.SampleDesc.Count = 4;
        m_SwapChainDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
    }
    else
    {
        m_SwapChainDesc.SampleDesc.Count = 1;
        m_SwapChainDesc.SampleDesc.Quality = 0;
    }
    m_SwapChainDesc.OutputWindow = m_hMainWnd;
    m_SwapChainDesc.Windowed = true;
    m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_SwapChainDesc.Flags = 0;

    //Create swap chain
    IDXGIDevice *dxgiDevice = 0;
    ValidateResult(m_pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

    IDXGIAdapter *dxgiAdapter = 0;
    ValidateResult(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

    ValidateResult(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_pdxgiFactory));

    ValidateResult(m_pdxgiFactory->CreateSwapChain(m_pD3DDevice, &m_SwapChainDesc, &m_pSwapChain));

    ReleaseCOM(dxgiAdapter);
    ReleaseCOM(dxgiDevice);
}

void XC_GraphicsDx11::SetupRenderTargets()
{
    //Initiate RenderableTextures
    m_renderTargets[RENDERTARGET_MAIN_0] = XCNEW(RenderableTexture)(RENDERTARGET_MAIN_0, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_MAIN_0]->PreLoad(m_pSwapChain);

    m_renderTargets[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL] = XCNEW(RenderableTexture)(RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]->PreLoad(m_Enable4xMsaa && m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

    m_renderTargets[RENDERTARGET_GBUFFER_LIGHTING] = XCNEW(RenderableTexture)(RENDERTARGET_GBUFFER_LIGHTING, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_GBUFFER_LIGHTING]->PreLoad(m_Enable4xMsaa && m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

    m_renderTargets[RENDERTARGET_LIVEDRIVE] = XCNEW(RenderableTexture)(RENDERTARGET_LIVEDRIVE, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->PreLoad(m_Enable4xMsaa && m_4xMsaaQuality, 256, 256);
}

void XC_GraphicsDx11::SetupRenderQuad()
{
    m_renderQuadVB = XCNEW(VertexBuffer<VertexPosTex>);
    m_renderQuadIB = XCNEW(IndexBuffer<u32>);

    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(-1.0f, 1.0f, 0.0f), XCVec2Unaligned(0.0f, 0.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(1.0f, -1.0f, 0.0f), XCVec2Unaligned(1.0f, 1.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(-1.0f, -1.0f, 0.0f), XCVec2Unaligned(0.0f, 1.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(1.0f, 1.0f, 0.0f), XCVec2Unaligned(1.0f, 0.0f)));

    m_renderQuadVB->BuildVertexBuffer(m_pD3DDeviceContext);

    //Set up index buffer
    m_renderQuadIB->AddIndicesVA
    ({
        0, 1, 2,
        0, 3, 1,
    });

    m_renderQuadIB->BuildIndexBuffer(m_pD3DDeviceContext);
}

void XC_GraphicsDx11::SetupDepthStencilStates()
{
    //Create depth Stencil view
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    ValidateResult(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState));

    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc = { 0 };

    //Create DepthView with depth disabled
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    ValidateResult(m_pD3DDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState));

    //Create depth Stencil view, with D3D11_less_equal for skybox
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // Create the state using the device.
    ValidateResult(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilLessEqualState));
}

void XC_GraphicsDx11::SetupDepthView()
{
    //Create Depth Buffer & view
    D3D_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (m_Enable4xMsaa)
    {
        depthBufferDesc.SampleDesc.Count = 4;
        depthBufferDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
    }
    else
    {
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
    }

    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RENDERTARGET_MAIN_0].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RENDERTARGET_MAIN_0].Height;
    m_depthStencilResource[RENDERTARGET_MAIN_0] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_MAIN_0]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL].Height;
    m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RENDERTARGET_GBUFFER_LIGHTING].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RENDERTARGET_GBUFFER_LIGHTING].Height;
    m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RENDERTARGET_LIVEDRIVE].Height;
    m_depthStencilResource[RENDERTARGET_LIVEDRIVE] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_LIVEDRIVE]);
}

void XC_GraphicsDx11::SetupViewPort()
{
    XC_Graphics::SetupViewPort();
}

void XC_GraphicsDx11::Update(f32 dt)
{
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->Update();
}

void XC_GraphicsDx11::BeginScene()
{
    m_pD3DDeviceContext->RSSetViewports(1, &m_ScreenViewPort[RENDERTARGET_MAIN_0]);

    m_renderTargets[RENDERTARGET_MAIN_0]->SetRenderableTarget(*m_pD3DDeviceContext, m_depthStencilResource[RENDERTARGET_MAIN_0]);
    m_renderTargets[RENDERTARGET_MAIN_0]->ClearRenderTarget(*m_pD3DDeviceContext, m_depthStencilResource[RENDERTARGET_MAIN_0], m_clearColor);

    m_renderingPool->Begin();
}

void XC_GraphicsDx11::BeginSecondaryScene()
{
    SetSecondaryDrawCall(true);
    
    //TurnOffZ();
    m_renderingPool->Begin();
}

void XC_GraphicsDx11::EndSecondaryScene()
{
    //TurnOnZ();
    SetSecondaryDrawCall(false);
}

void XC_GraphicsDx11::EndScene()
{
    m_renderingPool->End();

    //Draw post processed render target
    m_XCShaderSystem->ApplyShader(*m_pD3DDeviceContext, ShaderType_RenderTexture);

    XCShaderHandle* shaderHandle = (XCShaderHandle*)m_XCShaderSystem->GetShader(ShaderType_RenderTexture);
    shaderHandle->SetResource("gTexture", *m_pD3DDeviceContext, m_renderTargets[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]->GetRenderTargetResource());

    shaderHandle->SetVertexBuffer(*m_pD3DDeviceContext, m_renderQuadVB);
    shaderHandle->SetIndexBuffer(*m_pD3DDeviceContext, *m_renderQuadIB);

    m_pD3DDeviceContext->DrawIndexedInstanced(m_renderQuadIB->m_indexData.size(), 1, 0, 0, 0);

    ValidateResult(m_pSwapChain->Present(1, 0));
}

void XC_GraphicsDx11::TurnOnZ()
{
    m_pD3DDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void XC_GraphicsDx11::TurnOffZ()
{
    m_pD3DDeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void XC_GraphicsDx11::SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn)
{
    if (turnOn)
        context.OMSetDepthStencilState(m_depthStencilLessEqualState, 1);
    else
        context.OMSetDepthStencilState(m_depthStencilState, 1);
}

void XC_GraphicsDx11::GoFullscreen(bool go)
{
    m_pSwapChain->SetFullscreenState(go, nullptr);
}

void XC_GraphicsDx11::OnResize(i32 _width, i32 _height)
{
    if (m_initDone)
    {
        m_ClientWidth = _width;
        m_ClientHeight = _height;

        m_renderTargets[RENDERTARGET_MAIN_0]->OnResize();

        m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

        //Reset the Screen ViewPort
        SetupViewPort();

        //Create RenderTargetView
        m_renderTargets[RENDERTARGET_MAIN_0]->PreLoad(m_pSwapChain);

        //Reset the depth stencil
        D3D_TEXTURE2D_DESC depthDesc = {};
        m_depthStencilResource[RENDERTARGET_MAIN_0]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RENDERTARGET_MAIN_0]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RENDERTARGET_MAIN_0] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_MAIN_0]);

        m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_GBUFFER_POS_DIFFUSE_NORMAL]);

        m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RENDERTARGET_GBUFFER_LIGHTING]);
    }
}
#endif