/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_DX11)

#include "XC_GraphicsDx11.h"
#include "Graphics/XC_Shaders/XC_VertexShaderLayout.h"

XC_GraphicsDx11::XC_GraphicsDx11(void)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_pdxgiFactory = 0;
    m_pD3DDeviceContext = 0;
    m_pD3DDevice = 0;
    m_pRenderTargetView = 0;
    m_pDepthStencilBuffer = 0;
    m_pDepthStencilView = 0;
    m_depthDisabledStencilState = 0;
    m_XCShaderSystem = nullptr;

    ZeroMemory(&m_ScreenViewPort, sizeof(D3D_VIEWPORT));
    m_initDone = false;
}

XC_GraphicsDx11::~XC_GraphicsDx11(void)
{
    delete(m_XCShaderSystem);
    delete(m_renderingPool);
    delete(m_renderTargets[RENDERTARGET_MAIN_0]);
    delete(m_renderTargets[RENDERTARGET_LIVEDRIVE]);
}

void XC_GraphicsDx11::Destroy()
{
    m_XCShaderSystem->Destroy();
    m_renderingPool->Destroy();

    m_renderTargets[RENDERTARGET_MAIN_0]->Destroy();
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->Destroy();

    ReleaseCOM(m_pD3DDeviceContext);
    ReleaseCOM(m_pD3DDevice);
    ReleaseCOM(m_pdxgiFactory);
    ReleaseCOM(m_pSwapChain);
}

void XC_GraphicsDx11::InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa)
{
    XC_Graphics::InitGraphicsWindow(_mainWnd, _width, _height, _enable4xMsaa);

    SetupPipeline();
    m_initDone = true;
}

void XC_GraphicsDx11::SetupPipeline()
{
    SetupDevice();
    SetupSwapChain();
    SetupRenderTargets();
    SetupDepthStencilBuffer();
    SetupDepthStencilStates();
    SetupDepthView();
    SetupViewPort();

    SetupShadersAndRenderPool();
}

void XC_GraphicsDx11::SetupDevice()
{
    unsigned int createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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
    if (m_4xMsaaQuality)
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
    //Create RenderTargetView
    ID3DTexture2D* backBuffer;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3DTexture2D), (void**)&backBuffer);

    //Initiate RenderableTextures
    m_renderTargets[RENDERTARGET_MAIN_0] = new RenderableTexture(*m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_MAIN_0]->PreLoad(backBuffer);

    m_renderTargets[RENDERTARGET_LIVEDRIVE] = new RenderableTexture(*m_pD3DDevice, *m_pD3DDeviceContext);
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->PreLoad(m_4xMsaaQuality, 256, 256);

    ReleaseCOM(backBuffer);
}

void XC_GraphicsDx11::SetupDepthStencilBuffer()
{
    //Create Depth Buffer & view
    m_depthBufferDesc.Width = m_ClientWidth;
    m_depthBufferDesc.Height = m_ClientHeight;
    m_depthBufferDesc.MipLevels = 1;
    m_depthBufferDesc.ArraySize = 1;
    m_depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (m_Enable4xMsaa)
    {
        m_depthBufferDesc.SampleDesc.Count = 4;
        m_depthBufferDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
    }
    else
    {
        m_depthBufferDesc.SampleDesc.Count = 1;
        m_depthBufferDesc.SampleDesc.Quality = 0;
    }

    m_depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_depthBufferDesc.CPUAccessFlags = 0;
    m_depthBufferDesc.MiscFlags = 0;

    ValidateResult(m_pD3DDevice->CreateTexture2D(&m_depthBufferDesc, 0, &m_pDepthStencilBuffer));

    //Create Depth buffer & view for live drive
    m_depthBufferDescLiveDrive.Width = 256;
    m_depthBufferDescLiveDrive.Height = 256;
    m_depthBufferDescLiveDrive.MipLevels = 1;
    m_depthBufferDescLiveDrive.ArraySize = 1;
    m_depthBufferDescLiveDrive.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (m_Enable4xMsaa)
    {
        m_depthBufferDescLiveDrive.SampleDesc.Count = 4;
        m_depthBufferDescLiveDrive.SampleDesc.Quality = m_4xMsaaQuality - 1;
    }
    else
    {
        m_depthBufferDescLiveDrive.SampleDesc.Count = 1;
        m_depthBufferDescLiveDrive.SampleDesc.Quality = 0;
    }

    m_depthBufferDescLiveDrive.Usage = D3D11_USAGE_DEFAULT;
    m_depthBufferDescLiveDrive.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_depthBufferDescLiveDrive.CPUAccessFlags = 0;
    m_depthBufferDescLiveDrive.MiscFlags = 0;

    ValidateResult(m_pD3DDevice->CreateTexture2D(&m_depthBufferDescLiveDrive, 0, &m_pDepthStencilBufferLiveDrive));
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
    // Set up the depth stencil view description.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    ValidateResult(m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView));

    //Binding the view to the output merger stage
    m_renderTargets[RENDERTARGET_MAIN_0]->SetRenderableTarget(*m_pD3DDeviceContext, m_pDepthStencilView);

    // Create the depth stencil view.
    ValidateResult(m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBufferLiveDrive, &depthStencilViewDesc, &m_pDepthStencilViewLiveDrive));

    //Binding the view to the output merger stage for Live Drive
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->SetRenderableTarget(*m_pD3DDeviceContext, m_pDepthStencilViewLiveDrive);
}

void XC_GraphicsDx11::SetupViewPort()
{
    XC_Graphics::SetupViewPort();
}

void XC_GraphicsDx11::Update(float dt)
{
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->Update();
}

void XC_GraphicsDx11::BeginScene()
{
    m_pD3DDeviceContext->RSSetViewports(1, &m_ScreenViewPort[RENDERTARGET_MAIN_0]);

    m_renderTargets[RENDERTARGET_MAIN_0]->SetRenderableTarget(*m_pD3DDeviceContext, m_pDepthStencilView);
    m_renderTargets[RENDERTARGET_MAIN_0]->ClearRenderTarget(*m_pD3DDeviceContext, m_pDepthStencilView, m_clearColor);

    m_renderingPool->Begin(RENDERTARGET_MAIN_0);
}

void XC_GraphicsDx11::BeginSecondaryScene()
{
    SetSecondaryDrawCall(true);

    m_pD3DDeviceContext->RSSetViewports(1, &m_ScreenViewPort[RENDERTARGET_LIVEDRIVE]);

    //TurnOffZ();
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->SetRenderableTarget(*m_pD3DDeviceContext, m_pDepthStencilViewLiveDrive);
    m_renderTargets[RENDERTARGET_LIVEDRIVE]->ClearRenderTarget(*m_pD3DDeviceContext, m_pDepthStencilViewLiveDrive, XCVec4(0.0f, 1.0f, 0.0f, 0.0f));

    m_renderingPool->Begin(RENDERTARGET_LIVEDRIVE);
}

void XC_GraphicsDx11::EndSecondaryScene()
{
    //TurnOnZ();
    SetSecondaryDrawCall(false);
}

void XC_GraphicsDx11::EndScene()
{
    m_renderingPool->End();
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

void XC_GraphicsDx11::OnResize(int _width, int _height)
{
    if (m_initDone)
    {
        m_ClientWidth = _width;
        m_ClientHeight = _height;

        m_pDepthStencilBuffer->Release();
        m_renderTargets[RENDERTARGET_MAIN_0]->OnResize();
        m_pDepthStencilView->Release();

        m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

        //Create RenderTargetView
        ID3DTexture2D *backBuffer;

        m_pSwapChain->GetBuffer(0, __uuidof(ID3DTexture2D), reinterpret_cast<void**>(&backBuffer));
        m_renderTargets[RENDERTARGET_MAIN_0]->PreLoad(backBuffer);

        ReleaseCOM(backBuffer);

        //Reset the Backbuffer and depth buffer
        m_depthBufferDesc.Width = m_ClientWidth;
        m_depthBufferDesc.Height = m_ClientHeight;

        ValidateResult(m_pD3DDevice->CreateTexture2D(&m_depthBufferDesc, 0, &m_pDepthStencilBuffer));
        ValidateResult(m_pD3DDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));

        //Binding the view to the output merger stage
        m_renderTargets[RENDERTARGET_MAIN_0]->SetRenderableTarget(*m_pD3DDeviceContext, m_pDepthStencilView);

        //Reset the Screen ViewPort
        SetupViewPort();
    }
}
#endif