/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_DX11)

#include "XCGraphicsDx11.h"

#include "Graphics/RenderingPool.h"
#include "Graphics/XCShaders/XCVertexShaderLayout.h"
#include "Graphics/XCTextures/RenderableTexture.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/GPUResourceSystem.h"
#include "Graphics/SharedDescriptorHeap.h"

 //TODO : Remove this dependency of light over here.
#include "Graphics/XCLighting/XCLightManager.h"

XCGraphicsDx11::XCGraphicsDx11(void)
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
    m_frameIndex = RenderTargetType_Main_0;
}

XCGraphicsDx11::~XCGraphicsDx11(void)
{
}

void XCGraphicsDx11::Destroy()
{
    XCGraphics::Destroy();

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

void XCGraphicsDx11::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    XCGraphics::Init(_mainWnd, _width, _height, _enable4xMsaa);

    SetupPipeline();
    m_initDone = true;
}

void XCGraphicsDx11::SetupPipeline()
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

void XCGraphicsDx11::CreateDescriptorHeaps()
{
    //Initialize shader shared descriptor heap
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_sharedDescriptorHeap = (SharedDescriptorHeap*)&container.CreateNewSystem("SharedDescriptorHeap");

    m_sharedDescriptorHeap->Init(RenderTargetType_Max + 1 //No of RTV's
        , RenderTargetType_Max + 1 //No of DSV's
        , 1 //No of Samplers
        , 100 //No of CBV, UAV combined
    );
}

void XCGraphicsDx11::CreateGPUResourceSystem()
{
    //GPU Resource system
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<GPUResourceSystem>("GPUResourceSystem");
    m_gpuResourceSystem = (GPUResourceSystem*) &container.CreateNewSystem("GPUResourceSystem");
    m_gpuResourceSystem->Init();
}

void XCGraphicsDx11::SetupDevice()
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

void XCGraphicsDx11::SetupSwapChain()
{
    //Check for Multi Sampling quality
    ValidateResult(m_pD3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));

    //Describing Swap Chains
    m_SwapChainDesc.BufferDesc.Width = m_clientWidth;
    m_SwapChainDesc.BufferDesc.Height = m_clientHeight;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.BufferCount = 1;
    if (m_enable4xMsaa && m_4xMsaaQuality)
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

void XCGraphicsDx11::SetupRenderTargets()
{
    //Initiate RenderableTextures
    m_renderTargets[RenderTargetType_Main_0] = XCNEW(RenderableTexture)(RenderTargetType_Main_0, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RenderTargetType_Main_0]->PreLoad(m_pSwapChain);

    m_renderTargets[RenderTargetType_GBuffer_Diffuse] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Diffuse, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RenderTargetType_GBuffer_Diffuse]->PreLoad(m_enable4xMsaa && m_4xMsaaQuality, m_clientWidth, m_clientHeight);

    m_renderTargets[RenderTargetType_GBuffer_Position] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Position, DXGI_FORMAT_R32G32B32A32_FLOAT, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RenderTargetType_GBuffer_Position]->PreLoad(m_enable4xMsaa && m_4xMsaaQuality, m_clientWidth, m_clientHeight);

    m_renderTargets[RenderTargetType_GBuffer_Normal] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Normal, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RenderTargetType_GBuffer_Normal]->PreLoad(m_enable4xMsaa && m_4xMsaaQuality, m_clientWidth, m_clientHeight);

    m_renderTargets[RenderTargetType_LiveDrive] = XCNEW(RenderableTexture)(RenderTargetType_LiveDrive, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RenderTargetType_LiveDrive]->PreLoad(m_enable4xMsaa && m_4xMsaaQuality, 256, 256);
}

void XCGraphicsDx11::SetupRenderQuad()
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

void XCGraphicsDx11::SetupDepthStencilStates()
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

void XCGraphicsDx11::SetupDepthView()
{
    //Create Depth Buffer & view
    D3D_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (m_enable4xMsaa)
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

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_Main_0].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_Main_0].Height;
    m_depthStencilResource[RenderTargetType_Main_0] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Main_0]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Diffuse].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Diffuse].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Diffuse] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Position].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Position].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Position] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Position]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Normal].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Normal].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Normal] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Normal]);

    depthBufferDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_LiveDrive].Width;
    depthBufferDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_LiveDrive].Height;
    m_depthStencilResource[RenderTargetType_LiveDrive] = m_gpuResourceSystem->CreateTextureResource(depthBufferDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_LiveDrive]);
}

void XCGraphicsDx11::SetupViewPort()
{
    XCGraphics::SetupViewPort();
}

void XCGraphicsDx11::Update(f32 dt)
{
    m_renderTargets[RenderTargetType_LiveDrive]->Update();
}

void XCGraphicsDx11::BeginScene()
{
    m_pD3DDeviceContext->RSSetViewports(1, &m_ScreenViewPort[RenderTargetType_Main_0]);

    //On immediate context, we only want to work on current frame render target, so set and clear
    std::vector<RenderTargetsType> rtvs = { (RenderTargetsType)m_frameIndex };

    SetRenderableTargets(*m_pD3DDeviceContext, rtvs);
    ClearRTVAndDSVs(*m_pD3DDeviceContext, rtvs, XCVec4(1.0f, 1.0f, 1.0f, 1.0f));

    //On deferred context, we work on multiple render targets for deferred lighting. So work on them.
    rtvs.push_back(RenderTargetType_GBuffer_Diffuse);
    rtvs.push_back(RenderTargetType_GBuffer_Position);
    rtvs.push_back(RenderTargetType_GBuffer_Normal);

    m_renderingPool->Begin(rtvs);
}

#pragma region LiveDriveRendering
void XCGraphicsDx11::BeginSecondaryScene()
{
    SetSecondaryDrawCall(true);
    
    std::vector<RenderTargetsType> rtvs = { (RenderTargetsType)m_frameIndex };

    //TurnOffZ();
    m_renderingPool->Begin(rtvs);
}

void XCGraphicsDx11::EndSecondaryScene()
{
    //TurnOnZ();
    SetSecondaryDrawCall(false);
}
#pragma endregion

void XCGraphicsDx11::EndScene()
{
    m_renderingPool->End();

    //Draw post processed render target
    m_xcShaderSystem->ApplyShader(*m_pD3DDeviceContext, ShaderType_DeferredLighting);

    XCShaderHandle* shaderHandle = (XCShaderHandle*)m_xcShaderSystem->GetShader(ShaderType_DeferredLighting);
    shaderHandle->SetResource("gGBufferDiffuse", *m_pD3DDeviceContext, m_renderTargets[RenderTargetType_GBuffer_Diffuse]->GetRenderTargetResource());
    shaderHandle->SetResource("gGBufferPosition", *m_pD3DDeviceContext, m_renderTargets[RenderTargetType_GBuffer_Position]->GetRenderTargetResource());
    shaderHandle->SetResource("gGBufferNormal", *m_pD3DDeviceContext, m_renderTargets[RenderTargetType_GBuffer_Normal]->GetRenderTargetResource());

    shaderHandle->SetVertexBuffer(*m_pD3DDeviceContext, m_renderQuadVB);
    shaderHandle->SetIndexBuffer(*m_pD3DDeviceContext, *m_renderQuadIB);

    XCLightManager* lightMgr = (XCLightManager*)&SystemLocator::GetInstance()->RequestSystem("LightsManager");
    shaderHandle->SetConstantBuffer("cbLightsPerFrame", *m_pD3DDeviceContext, lightMgr->GetLightConstantBuffer());

    m_pD3DDeviceContext->DrawIndexedInstanced(m_renderQuadIB->m_indexData.size(), 1, 0, 0, 0);


    //Present
    ValidateResult(m_pSwapChain->Present(1, 0));
}

void XCGraphicsDx11::SetRenderableTargets(ID3DDeviceContext& context, const std::vector<RenderTargetsType>& types)
{
    //Create a list of all the rtv's & dsv's that are going to be used
    ID3D11RenderTargetView*     rtvDescHandle[RenderTargetType_Max];
    ID3D11DepthStencilView*     depthDescHandle[RenderTargetType_Max];

    for (u32 rIndex = 0; rIndex < types.size(); ++rIndex)
    {
        rtvDescHandle[rIndex] = m_renderTargets[types[rIndex]]->GetRenderTargetResource()->GetPointerToGPUResourceViewTyped<ID3D11RenderTargetView*>(GPUResourceType_RTV);
        depthDescHandle[rIndex] = m_depthStencilResource[types[rIndex]]->GetPointerToGPUResourceViewTyped<ID3D11DepthStencilView*>(GPUResourceType_DSV);
    }

    context.OMSetRenderTargets(types.size(), &rtvDescHandle[0], depthDescHandle[0]);
}

void XCGraphicsDx11::ClearRTVAndDSVs(ID3DDeviceContext& context, std::vector<RenderTargetsType>& type, XCVec4& clearColor)
{
    const f32 color[] = { clearColor.Get<X>(), clearColor.Get<Y>(), clearColor.Get<Z>(), clearColor.Get<W>() };

    for (u32 rIndex = 0; rIndex < type.size(); ++rIndex)
    {
        context.ClearRenderTargetView(m_renderTargets[type[rIndex]]->GetRenderTargetResource()->GetGPUResourceViewTyped<ID3D11RenderTargetView*>(GPUResourceType_RTV), color);
        context.ClearDepthStencilView(m_depthStencilResource[type[rIndex]]->GetGPUResourceViewTyped<ID3DDepthStencilView*>(GPUResourceType_DSV), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void XCGraphicsDx11::TurnOnZ()
{
    m_pD3DDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}

void XCGraphicsDx11::TurnOffZ()
{
    m_pD3DDeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}

void XCGraphicsDx11::SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn)
{
    if (turnOn)
        context.OMSetDepthStencilState(m_depthStencilLessEqualState, 1);
    else
        context.OMSetDepthStencilState(m_depthStencilState, 1);
}

void XCGraphicsDx11::GoFullscreen(bool go)
{
    m_pSwapChain->SetFullscreenState(go, nullptr);
}

void XCGraphicsDx11::OnResize(i32 _width, i32 _height)
{
    if (m_initDone)
    {
        m_clientWidth = _width;
        m_clientHeight = _height;

        m_renderTargets[RenderTargetType_Main_0]->OnResize();

        m_pSwapChain->ResizeBuffers(1, m_clientWidth, m_clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

        //Reset the Screen ViewPort
        SetupViewPort();

        //Create RenderTargetView
        m_renderTargets[RenderTargetType_Main_0]->PreLoad(m_pSwapChain);

        //Reset the depth stencil
        D3D_TEXTURE2D_DESC depthDesc = {};
        m_depthStencilResource[RenderTargetType_Main_0]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_Main_0]);
        depthDesc.Width = m_clientWidth;
        depthDesc.Height = m_clientHeight;
        m_depthStencilResource[RenderTargetType_Main_0] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Main_0]);

        m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);
        depthDesc.Width = m_clientWidth;
        depthDesc.Height = m_clientHeight;
        m_depthStencilResource[RenderTargetType_GBuffer_Diffuse] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);

        m_depthStencilResource[RenderTargetType_GBuffer_Position]->GetResource<ID3D11Texture2D*>()->GetDesc(&depthDesc);
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_GBuffer_Position]);
        depthDesc.Width = m_clientWidth;
        depthDesc.Height = m_clientHeight;
        m_depthStencilResource[RenderTargetType_GBuffer_Position] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Position]);
    }
}
#endif