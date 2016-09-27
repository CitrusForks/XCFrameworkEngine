/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_DX12)

#include <dxgi.h>

#include "XC_GraphicsDx12.h"

#include "Graphics/XC_Shaders/XC_VertexShaderLayout.h"
#include "Graphics/SharedDescriptorHeap.h"
#include "Graphics/GPUResourceSystem.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"

//TODO : Remove this dependency of light over here.
#include "Graphics/XC_Lighting/XC_LightManager.h"

#include "Assets/Packages/PackageConsts.h"
#include "Libs/Dx12Helpers/d3dx12.h"

XC_GraphicsDx12::XC_GraphicsDx12(void)
    : m_pSwapChain(nullptr)
    , m_pdxgiFactory(nullptr)
    , m_pCommandAllocator(nullptr)
    , m_pCommandQueue(nullptr)
    , m_graphicsCommandList(nullptr)
    , m_pFence(nullptr)
    , m_rootSignature(nullptr)
    , m_pipelineState(nullptr)
    , m_renderQuadVB(nullptr)
    , m_renderQuadIB(nullptr)
{
}

XC_GraphicsDx12::~XC_GraphicsDx12(void)
{
}

void XC_GraphicsDx12::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    XC_Graphics::Init(_mainWnd, _width, _height, _enable4xMsaa);

    SetWindowText(_mainWnd, "XC Framework Dx12");

    //Initialize some dxgi factories.
    ValidateResult(CreateDXGIFactory1(IID_PPV_ARGS(&m_pdxgiFactory)));

#ifdef _DEBUG
    ID3D12Debug* debugInterface;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
    {
        debugInterface->EnableDebugLayer();
    }
#endif
    
    u32 adapterIndex = 0;
    IDXGIAdapter* pAdapter;
    HRESULT hr;

    while (m_pdxgiFactory->EnumAdapters(adapterIndex++, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        hr = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pD3DDevice));
        if (hr == S_OK)
        {
            break;
        }
    }

    if (FAILED(hr))
    {
        MessageBox(0, "Direct 3D 12 Create Hardware Failed", 0, 0);
        
        //Try WARP MODE
        IDXGIAdapter* wrapAdapter;

        ValidateResult(m_pdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&wrapAdapter)));

        HRESULT hr = D3D12CreateDevice(wrapAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pD3DDevice));

        if (FAILED(hr))
        {
            MessageBox(0, "Direct 3D 12 Create Hardware Failed with reduced hardware feature level", 0, 0);
            PostQuitMessage(0);
        }
    }

    //Create main thread cmd allocator
    ValidateResult(m_pD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator)));

    //Create cmd Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ValidateResult(m_pD3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue)));

    //Describing Swap Chains
    m_SwapChainDesc = {};
    m_SwapChainDesc.BufferDesc.Width = m_ClientWidth;
    m_SwapChainDesc.BufferDesc.Height = m_ClientHeight;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.BufferCount = 2;
    
    m_SwapChainDesc.OutputWindow = m_hMainWnd;
    m_SwapChainDesc.Windowed = true;
    m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    
    m_SwapChainDesc.Flags = 0;
    m_SwapChainDesc.SampleDesc.Count = 1;
    m_SwapChainDesc.SampleDesc.Quality = 0;

    //Create swap chain
    IDXGISwapChain* swapChain;
    hr = m_pdxgiFactory->CreateSwapChain(m_pCommandQueue, &m_SwapChainDesc, &swapChain);

    if (FAILED(hr))
    {
        MessageBox(0, "Direct 3D 12 Create swap chain failed", 0, 0);
        PostQuitMessage(0);
    }
    m_pSwapChain = (ID3DSwapChain*)swapChain;
    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    //Create Descriptor heaps.
    CreateDescriptorHeaps();
    CreateGPUResourceSystem();

    //Setup ViewPorts
    SetupViewPort();

    //Setup render targets
    SetupRenderTargets();

    //Create the depth view
    SetupDepthView();

    //Debug draw triangle
    DebugTestGraphicsPipeline();

    //Create Command list
    //This will be main thread immediate device context.
    m_pD3DDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_pCommandAllocator,
        m_pipelineState,
        IID_PPV_ARGS(&m_graphicsCommandList));

    //Setup the PipelineStateObjects
    SetupShadersAndRenderPool();

    //Create the render quad
    SetupRenderQuad();

    //Need to close the cmmd list as nothing is to be recorded now.
    ValidateResult(m_graphicsCommandList->Close());

    //Execute any pending work on m_graphicsCommandList.
    ID3D12CommandList* ppCmdList[] = { m_graphicsCommandList };
    m_pCommandQueue->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);

    //Create fencing for sync each frame
    ValidateResult(m_pD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(nullptr, false, false, "FrameSyncFenceEvent");
    if (m_fenceEvent == nullptr)
    {
        ValidateResult(HRESULT_FROM_WIN32(GetLastError()));
    }

    WaitForPreviousFrameCompletion();
    m_initDone = true;
}

void XC_GraphicsDx12::Destroy()
{
    XC_Graphics::Destroy();

    XCDELETE(m_renderQuadVB);
    XCDELETE(m_renderQuadIB);

    m_sharedDescriptorHeap->Destroy();
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RemoveSystem("SharedDescriptorHeap");

    ReleaseCOM(m_pCommandAllocator);
    ReleaseCOM(m_pCommandQueue);
    ReleaseCOM(m_graphicsCommandList);
    ReleaseCOM(m_rootSignature);
    ReleaseCOM(m_pipelineState);

#if defined(DEBUG_GRAPHICS_PIPELINE)
    ReleaseCOM(m_vertexBuffer);
#endif

    ReleaseCOM(m_pD3DDevice);
    ReleaseCOM(m_pdxgiFactory);
    ReleaseCOM(m_pSwapChain);
}

#pragma region Debug
void XC_GraphicsDx12::DebugTestGraphicsPipeline()
{
    // Create an empty root signature.
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob* signature;
        ID3DBlob* error;
        ValidateResult(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ValidateResult(m_pD3DDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        ID3DVertexShader* vertexShader;
        ID3DPixelShader* pixelShader;

#ifdef _DEBUG
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

#if defined(USE_D3D_COMPILER)
        ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\Default\\default.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ValidateResult(D3DCompileFromFile(L"Assets\\Shaders\\Default\\default.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
#else
        u32 vsSize, psSize;
        ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\Default\\default_vs.cso", &vertexShader, vsSize));
        ValidateResult(ReadRawDataFromFile("Assets\\Shaders\\Default\\default_ps.cso", &pixelShader, psSize));
        XCASSERT(vertexShader || pixelShader);
#endif
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature;
#if defined(USE_D3D_COMPILER)
        psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
#else
        psoDesc.VS = { vertexShader, vsSize };
        psoDesc.PS = { pixelShader, psSize };
#endif
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ValidateResult(m_pD3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    }

#if defined(DEBUG_GRAPHICS_PIPELINE)
    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        const f32 aspectRatio = 1024 / 786;
        Vertex triangleVertices[] =
        {
            { XCVec3{ 0.0f, 0.25f * aspectRatio, 0.0f },    XCVec4{ 1.0f, 0.0f, 0.0f, 1.0f } },
            { XCVec3{ 0.25f, -0.25f * aspectRatio, 0.0f },  XCVec4{ 0.0f, 1.0f, 0.0f, 1.0f } },
            { XCVec3{ -0.25f, -0.25f * aspectRatio, 0.0f }, XCVec4{ 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);

        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        ValidateResult(m_pD3DDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        ValidateResult(m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;
    }
#endif
}
#pragma endregion

void XC_GraphicsDx12::SetupRenderTargets()
{
    //Initiate RenderableTextures
    m_renderTargets[RenderTargetType_Main_0] = XCNEW(RenderableTexture)(RenderTargetType_Main_0, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_Main_0]->PreLoad(m_pSwapChain);

    m_renderTargets[RenderTargetType_Main_1] = XCNEW(RenderableTexture)(RenderTargetType_Main_1, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_Main_1]->PreLoad(m_pSwapChain);

    m_renderTargets[RenderTargetType_GBuffer_Diffuse] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Diffuse, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_GBuffer_Diffuse]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

    m_renderTargets[RenderTargetType_GBuffer_Position] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Position, DXGI_FORMAT_R32G32B32A32_FLOAT, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_GBuffer_Position]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

    m_renderTargets[RenderTargetType_GBuffer_Normal] = XCNEW(RenderableTexture)(RenderTargetType_GBuffer_Normal, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_GBuffer_Normal]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

#if defined(DEBUG_DEFERRED_LIGHTING)
    m_renderTargets[RenderTargetType_Debug] = XCNEW(RenderableTexture)(RenderTargetType_Debug, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_Debug]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);
#endif

    m_renderTargets[RenderTargetType_LiveDrive] = XCNEW(RenderableTexture)(RenderTargetType_LiveDrive, DXGI_FORMAT_R8G8B8A8_UNORM, *m_pD3DDevice, *m_graphicsCommandList);
    m_renderTargets[RenderTargetType_LiveDrive]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, 256, 256);
}

void XC_GraphicsDx12::SetupDepthView()
{
    D3D12_RESOURCE_DESC depthViewResDesc = {};
    depthViewResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthViewResDesc.Alignment = 0;
    depthViewResDesc.MipLevels = 1;
    depthViewResDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthViewResDesc.SampleDesc = { 1, 0 };
    depthViewResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthViewResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    depthViewResDesc.DepthOrArraySize = 1;

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_Main_0].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_Main_0].Height;
    m_depthStencilResource[RenderTargetType_Main_0] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Main_0]);

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_Main_1].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_Main_1].Height;
    m_depthStencilResource[RenderTargetType_Main_1] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Main_1]);

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Diffuse].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Diffuse].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Diffuse] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Position].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Position].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Position] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Position]);

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Normal].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_GBuffer_Normal].Height;
    m_depthStencilResource[RenderTargetType_GBuffer_Normal] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Normal]);

#if defined(DEBUG_DEFERRED_LIGHTING)
    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_Debug].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_Debug].Height;
    m_depthStencilResource[RenderTargetType_Debug] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Debug]);
#endif

    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RenderTargetType_LiveDrive].Width;
    depthViewResDesc.Height = (UINT64)m_ScreenViewPort[RenderTargetType_LiveDrive].Height;
    m_depthStencilResource[RenderTargetType_LiveDrive] = m_gpuResourceSystem->CreateTextureResource(depthViewResDesc, nullptr);
    m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_LiveDrive]);
}

void XC_GraphicsDx12::CreateDescriptorHeaps()
{
    //Initialize shader shared descriptor heap
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_sharedDescriptorHeap = (SharedDescriptorHeap*)&container.CreateNewSystem("SharedDescriptorHeap");

    m_sharedDescriptorHeap->Init(*m_pD3DDevice
        , RenderTargetType_Max + 1 //No of RTV's
        , RenderTargetType_Max + 1 //No of DSV's
        , 1 //No of Samplers
#if defined(LOAD_SHADERS_FROM_DATA)
        , 100 //No of CBV, UAV combined
#else
        SolidColorShader::NbOfDescriptors
        + LightTextureShader::NbOfDescriptors
        + TerrainMultiTex::NbOfDescriptors
        + CubeMapShader::NbOfDescriptors
        + SkinnedCharacterShader::NbOfDescriptors
        + NbOfTextureResources
#endif
        + 100
    );
}

void XC_GraphicsDx12::CreateGPUResourceSystem()
{
    //GPU Resource system
    SystemContainer& container = SystemLocator::GetInstance()->GetSystemContainer();
    container.RegisterSystem<GPUResourceSystem>("GPUResourceSystem");
    m_gpuResourceSystem = (GPUResourceSystem*)&container.CreateNewSystem("GPUResourceSystem");
    m_gpuResourceSystem->Init(*m_pD3DDevice);
}

void XC_GraphicsDx12::SetupRenderQuad()
{
    m_renderQuadVB = XCNEW(VertexBuffer<VertexPosTex>);
    m_renderQuadIB = XCNEW(IndexBuffer<u32>);

    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(-1.0f, 1.0f, 0.0f), XCVec2Unaligned(0.0f, 0.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(1.0f, -1.0f, 0.0f), XCVec2Unaligned(1.0f, 1.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(-1.0f, -1.0f, 0.0f), XCVec2Unaligned(0.0f, 1.0f)));
    m_renderQuadVB->m_vertexData.push_back(VertexPosTex(XCVec3Unaligned(1.0f, 1.0f, 0.0f), XCVec2Unaligned(1.0f, 0.0f)));

    m_renderQuadVB->BuildVertexBuffer(m_graphicsCommandList);

    //Set up index buffer
    m_renderQuadIB->AddIndicesVA
    ({  
        0, 1, 2,
        0, 3, 1,
    });

    m_renderQuadIB->BuildIndexBuffer(m_graphicsCommandList);
}

void XC_GraphicsDx12::SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
    D3D12_RESOURCE_BARRIER barrierDesc = {};

    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Transition.pResource = resource;
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = StateBefore;
    barrierDesc.Transition.StateAfter = StateAfter;

    commandList->ResourceBarrier(1, &barrierDesc);
}

void XC_GraphicsDx12::Update(f32 dt)
{
}

void XC_GraphicsDx12::BeginScene()
{
    ValidateResult(m_pCommandAllocator->Reset());
    ValidateResult(m_graphicsCommandList->Reset(m_pCommandAllocator, m_pipelineState));

    m_graphicsCommandList->SetGraphicsRootSignature(m_rootSignature);
    m_graphicsCommandList->RSSetViewports(1, &m_ScreenViewPort[m_frameIndex]);
    m_graphicsCommandList->RSSetScissorRects(1, &m_scissorRect);

    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[m_frameIndex]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Diffuse]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Position]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Normal]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

#if defined(DEBUG_DEFERRED_LIGHTING)
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_Debug]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
#endif

    //On immediate context, we only want to work on current frame render target, so set and clear
    std::vector<RenderTargetsType> rtvs = { (RenderTargetsType) m_frameIndex };

    SetRenderableTargets(*m_graphicsCommandList, rtvs);
    ClearRTVAndDSVs(*m_graphicsCommandList, rtvs, XCVec4(1.0f, 1.0f, 1.0f, 1.0f));

    //Set descriptor heaps
    ID3D12DescriptorHeap* ppHeaps[] = { m_sharedDescriptorHeap->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).m_heapDesc,
    m_sharedDescriptorHeap->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).m_heapDesc};
    m_graphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#if defined(DEBUG_GRAPHICS_PIPELINE)
    //Record ur cmds
    m_graphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_graphicsCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_graphicsCommandList->DrawInstanced(3, 1, 0, 0);
#endif

    //On deferred context, we work on multiple render targets for deferred lighting. So work on them.
    rtvs.push_back(RenderTargetType_GBuffer_Diffuse);
    rtvs.push_back(RenderTargetType_GBuffer_Position);
    rtvs.push_back(RenderTargetType_GBuffer_Normal);

    m_renderingPool->Begin(rtvs);
}

void XC_GraphicsDx12::EndScene()
{
    PIXSetMarker(m_pCommandQueue, 0, L"Scene End.");

    m_renderingPool->End();

    //Execute rendering pool context's
    m_renderingPool->Execute(m_pCommandQueue);

    //Draw post processed render target
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Diffuse]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Position]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Normal]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

#if defined(DEBUG_DEFERRED_LIGHTING)
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[RenderTargetType_Debug]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
#endif

    m_XCShaderSystem->ApplyShader(*m_graphicsCommandList, ShaderType_DeferredLighting);

    XCShaderHandle* shaderHandle = (XCShaderHandle*)m_XCShaderSystem->GetShader(ShaderType_DeferredLighting);
    shaderHandle->SetResource("gGBufferDiffuse", *m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Diffuse]->GetRenderTargetResource());
    shaderHandle->SetResource("gGBufferPosition", *m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Position]->GetRenderTargetResource());
    shaderHandle->SetResource("gGBufferNormal", *m_graphicsCommandList, m_renderTargets[RenderTargetType_GBuffer_Normal]->GetRenderTargetResource());

    shaderHandle->SetVertexBuffer(*m_graphicsCommandList, m_renderQuadVB);
    shaderHandle->SetIndexBuffer(*m_graphicsCommandList, *m_renderQuadIB);

    XC_LightManager* lightMgr = (XC_LightManager*)&SystemLocator::GetInstance()->RequestSystem("LightsManager");
    shaderHandle->SetConstantBuffer("cbLightsPerFrame", *m_graphicsCommandList, lightMgr->GetLightConstantBuffer());

    m_graphicsCommandList->DrawIndexedInstanced(m_renderQuadIB->m_indexData.size(), 1, 0, 0, 0);;

    //Present
    SetResourceBarrier(m_graphicsCommandList, m_renderTargets[m_frameIndex]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    ValidateResult(m_graphicsCommandList->Close());

    //Execute the cmd list
    ID3D12CommandList* ppCmdList[] = { m_graphicsCommandList };
    m_pCommandQueue->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);

    PIXBeginEvent(m_pCommandQueue, 0, L"Presenting to screen");
    ValidateResult(m_pSwapChain->Present(0, 0));
    PIXEndEvent(m_pCommandQueue);

    WaitForPreviousFrameCompletion();
}

void XC_GraphicsDx12::ClearRTVAndDSVs(ID3DDeviceContext& context, std::vector<RenderTargetsType>& type, XCVec4& clearColor)
{
    const f32 color[] = { clearColor.Get<X>(), clearColor.Get<Y>(), clearColor.Get<Z>(), clearColor.Get<W>() };

    for (u32 rIndex = 0; rIndex < type.size(); ++rIndex)
    {
        context.ClearRenderTargetView(m_renderTargets[type[rIndex]]->GetRenderTargetResource()->GetResourceView(GPUResourceType_RTV)->GetCPUResourceViewHandle()
            , color
            , 0
            , nullptr);

        context.ClearDepthStencilView(m_depthStencilResource[type[rIndex]]->GetResourceView(GPUResourceType_DSV)->GetCPUResourceViewHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }
}

void XC_GraphicsDx12::SetRenderableTargets(ID3DDeviceContext& context, const std::vector<RenderTargetsType>& types)
{
    //Create a list of all the rtv's & dsv's that are going to be used
    D3D12_CPU_DESCRIPTOR_HANDLE rtvDescHandle[RenderTargetType_Max];
    D3D12_CPU_DESCRIPTOR_HANDLE depthDescHandle[RenderTargetType_Max];

    for (u32 rIndex = 0; rIndex < types.size(); ++rIndex)
    {
        rtvDescHandle[rIndex]   = m_renderTargets[types[rIndex]]->GetRenderTargetResource()->GetResourceView(GPUResourceType_RTV)->GetCPUResourceViewHandle();
        depthDescHandle[rIndex] = m_depthStencilResource[types[rIndex]]->GetResourceView(GPUResourceType_DSV)->GetCPUResourceViewHandle();
    }

    context.OMSetRenderTargets(types.size(), &rtvDescHandle[0]
        , false  //this states that the input cpu handle is a array of handles pointing to rtvs
        , depthDescHandle);
}

void XC_GraphicsDx12::WaitForPreviousFrameCompletion()
{
    const UINT64 currentFenceValue = m_fenceValue;
    ValidateResult(m_pCommandQueue->Signal(m_pFence, m_fenceValue));
    m_fenceValue++;

    if (m_pFence->GetCompletedValue() < currentFenceValue)
    {
        ValidateResult(m_pFence->SetEventOnCompletion(currentFenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void XC_GraphicsDx12::OnResize(i32 _width, i32 _height)
{
    if (m_initDone)
    {
        m_ClientWidth = _width;
        m_ClientHeight = _height;

        m_renderTargets[RenderTargetType_Main_0]->OnResize();
        m_renderTargets[RenderTargetType_Main_1]->OnResize();

        m_renderTargets[RenderTargetType_Main_0]->Destroy();
        m_renderTargets[RenderTargetType_Main_1]->Destroy();
        m_renderTargets[RenderTargetType_GBuffer_Diffuse]->Destroy();
        m_renderTargets[RenderTargetType_GBuffer_Position]->Destroy();
        m_renderTargets[RenderTargetType_GBuffer_Normal]->Destroy();

        m_pSwapChain->ResizeBuffers(0, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        
        SetupViewPort();

        //Create RenderTargetView
        m_renderTargets[RenderTargetType_Main_0]->PreLoad(m_pSwapChain);
        m_renderTargets[RenderTargetType_Main_1]->PreLoad(m_pSwapChain);
        m_renderTargets[RenderTargetType_GBuffer_Diffuse]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);
        m_renderTargets[RenderTargetType_GBuffer_Position]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);
        m_renderTargets[RenderTargetType_GBuffer_Normal]->PreLoad(m_Enable4xMsaa & m_4xMsaaQuality, m_ClientWidth, m_ClientHeight);

        //Reset the depth stencil
        D3D_TEXTURE2D_DESC depthDesc = m_depthStencilResource[RenderTargetType_Main_0]->GetResource<ID3D12Resource*>()->GetDesc();
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_Main_0]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RenderTargetType_Main_0] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_Main_0]);

        depthDesc = m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]->GetResource<ID3D12Resource*>()->GetDesc();
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RenderTargetType_GBuffer_Diffuse] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Diffuse]);

        depthDesc = m_depthStencilResource[RenderTargetType_GBuffer_Position]->GetResource<ID3D12Resource*>()->GetDesc();
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_GBuffer_Position]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RenderTargetType_GBuffer_Position] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Position]);

        depthDesc = m_depthStencilResource[RenderTargetType_GBuffer_Normal]->GetResource<ID3D12Resource*>()->GetDesc();
        m_gpuResourceSystem->DestroyResource(m_depthStencilResource[RenderTargetType_GBuffer_Normal]);
        depthDesc.Width = m_ClientWidth;
        depthDesc.Height = m_ClientHeight;
        m_depthStencilResource[RenderTargetType_GBuffer_Normal] = m_gpuResourceSystem->CreateTextureResource(depthDesc, nullptr);
        m_gpuResourceSystem->CreateDepthStencilView(m_depthStencilResource[RenderTargetType_GBuffer_Normal]);
    }
}

#endif