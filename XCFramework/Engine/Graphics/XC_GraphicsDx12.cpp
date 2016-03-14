/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#if defined(XCGRAPHICS_DX12)

#include "XC_GraphicsDx12.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"
#include "Libs/Dx12Helpers/d3dx12.h"
#include <dxgi.h>

XC_GraphicsDx12::XC_GraphicsDx12(void)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_pdxgiFactory = 0;
    m_pD3DDevice = 0;
    m_frameIndex = 0;

    ZeroMemory(&m_ScreenViewPort, sizeof(D3D12_VIEWPORT));

    m_initDone = false;
}

XC_GraphicsDx12::~XC_GraphicsDx12(void)
{
    ReleaseCOM(m_pD3DDevice);
    ReleaseCOM(m_pdxgiFactory);
    ReleaseCOM(m_pSwapChain);
}

void XC_GraphicsDx12::InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa)
{
    XC_Graphics::InitGraphicsWindow(_mainWnd, _width, _height, _enable4xMsaa);

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
    
    unsigned int adapterIndex = 0;
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
    m_pSwapChain = (IDXGISwapChain3*)swapChain;
    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    //Setup ViewPorts
    SetupViewPort();

    //Setup render targets
    SetupRenderTargets();

    //Create Descriptor heaps.
    CreateDescriptorHeaps();

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
    //CreateGraphicPipelineStateObjects();
    SetupShadersAndRenderPool();

    //Need to close the cmmd list as nothing is to be recorded now.
    ValidateResult(m_graphicsCommandList->Close());

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
        unsigned int vsSize, psSize;
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
        const float aspectRatio = 1024 / 786;
        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f * aspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
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

void XC_GraphicsDx12::SetupRenderTargets()
{
    //Two render targets. Acts like swapping render targets.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ValidateResult(m_pD3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRTVDescriptorHeap)));

    m_rtvDescriptorSize = m_pD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    
    for (unsigned int frameIndex = 0; frameIndex < 2; ++frameIndex)
    {
        ValidateResult(m_pSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&m_renderTarget[frameIndex])));
        m_pD3DDevice->CreateRenderTargetView(m_renderTarget[frameIndex], nullptr, rtvCPUHandle);
        rtvCPUHandle.Offset(1, m_rtvDescriptorSize); //Move the handle to next rtv slot.
    }
}

void XC_GraphicsDx12::CreateDescriptorHeaps()
{
    //Create descriptor heap for constant buffers
    D3D12_DESCRIPTOR_HEAP_DESC cbHeapDesc = {};
    cbHeapDesc.NumDescriptors = 3;
    cbHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ValidateResult(m_pD3DDevice->CreateDescriptorHeap(&cbHeapDesc, IID_PPV_ARGS(&m_constantBuffersHeap)));
}

void XC_GraphicsDx12::CreateGraphicPipelineStateObjects()
{
    //Not required. Every shader will have its own PSO.
}

void XC_GraphicsDx12::SetupDepthView()
{
    //Allocate on heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ValidateResult(m_pD3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDSVDescriptorHeap)));

    D3D12_RESOURCE_DESC depthViewResDesc = {};
    depthViewResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthViewResDesc.Alignment = 0;
    depthViewResDesc.Width = (UINT64)m_ScreenViewPort[RENDERTARGET_MAIN_0].Width;
    depthViewResDesc.Height= (UINT64)m_ScreenViewPort[RENDERTARGET_MAIN_0].Height;
    depthViewResDesc.MipLevels = 1;
    depthViewResDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthViewResDesc.SampleDesc = {1, 0};
    depthViewResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthViewResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    depthViewResDesc.DepthOrArraySize = 1;

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    ValidateResult(m_pD3DDevice->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthViewResDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&m_depthStencilResource)));

    D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle = m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    m_pD3DDevice->CreateDepthStencilView(m_depthStencilResource, nullptr, dsvCPUHandle);

    //SetResourceBarrier(m_graphicsCommandList, m_depthStencilResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_READ);
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

void XC_GraphicsDx12::Update(float dt)
{
}

void XC_GraphicsDx12::BeginScene()
{
    ValidateResult(m_pCommandAllocator->Reset());
    ValidateResult(m_graphicsCommandList->Reset(m_pCommandAllocator, m_pipelineState));

    m_graphicsCommandList->SetGraphicsRootSignature(m_rootSignature);
    m_graphicsCommandList->RSSetViewports(1, &m_ScreenViewPort[RENDERTARGET_MAIN_0]);
    m_graphicsCommandList->RSSetScissorRects(1, &m_scissorRect);

    SetResourceBarrier(m_graphicsCommandList, m_renderTarget[m_frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    CPU_DESCRIPTOR_HANDLE handle = GetRTVCPUDescHandler();
    m_graphicsCommandList->OMSetRenderTargets(1, &handle, false, &m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    //Set descriptor heaps
    SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    ID3D12DescriptorHeap* ppHeaps[] = { heap.GetDescriptorHeap() };
    m_graphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#if defined(DEBUG_GRAPHICS_PIPELINE)
    //Record ur cmds
    m_graphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_graphicsCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_graphicsCommandList->DrawInstanced(3, 1, 0, 0);
#endif

    m_renderingPool->Begin(RENDERTARGET_MAIN_0);
}

void XC_GraphicsDx12::EndScene()
{
    //PIXSetMarker(m_pCommandQueue, 0, L"Scene End.");

    m_renderingPool->End();

    //Execute rendering pool context's
    m_renderingPool->Execute(m_pCommandQueue);

    SetResourceBarrier(m_graphicsCommandList, m_renderTarget[m_frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    ValidateResult(m_graphicsCommandList->Close());

    //Execute the cmd list
    ID3D12CommandList* ppCmdList[] = { m_graphicsCommandList };
    m_pCommandQueue->ExecuteCommandLists(_countof(ppCmdList), ppCmdList);

    //PIXBeginEvent(m_pCommandQueue, 0, L"Presenting to screen");
    ValidateResult(m_pSwapChain->Present(0, 0));
    //PIXEndEvent(m_pCommandQueue);

    WaitForPreviousFrameCompletion();
}

void XC_GraphicsDx12::PresentRenderTarget(ID3D12GraphicsCommandList* cmdList)
{
    SetResourceBarrier(cmdList, m_renderTarget[m_frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void XC_GraphicsDx12::ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList)
{
    //Clear screen
    float variadic = 1.0f /*+ (float)(0.5 * (float)(rand() % 5) / 10)*/;
    const float clearColor[] = { variadic * m_clearColor.x, variadic * m_clearColor.y, variadic * m_clearColor.z, m_clearColor.w };

    cmdList->ClearRenderTargetView(GetRTVCPUDescHandler(), clearColor, 0, nullptr);
    cmdList->ClearDepthStencilView(m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

CPU_DESCRIPTOR_HANDLE XC_GraphicsDx12::GetRTVCPUDescHandler()
{
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.ptr = rtvHandle.ptr + (m_frameIndex * m_rtvDescriptorSize);
    return rtvHandle;
}

CPU_DESCRIPTOR_HANDLE XC_GraphicsDx12::GetDSVCPUDescHandler()
{
    return m_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
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

void XC_GraphicsDx12::OnResize(int _width, int _height)
{
#if defined(WIN_32)
    if (m_initDone)
    {
        m_ClientWidth = _width;
        m_ClientHeight = _height;

        m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
        
        SetupViewPort();
    }
#endif
}

#endif