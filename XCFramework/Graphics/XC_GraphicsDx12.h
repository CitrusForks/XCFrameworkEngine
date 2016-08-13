/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)

#include "Graphics/XC_Graphics.h"
#include "Engine/Thread/CriticalSection.h"

class XC_GraphicsDx12 : public XC_Graphics
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XC_GraphicsDx12)

#if defined(DEBUG_GRAPHICS_PIPELINE)
    struct Vertex
    {
        XCVec3 position;
        XCVec4 color;
    };
#endif

    XC_GraphicsDx12(void);
    ~XC_GraphicsDx12(void);
    
    void                        Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    void                        BeginScene();
    void                        EndScene();
    void                        Update(f32 dt);
    void                        Destroy();

    ID3DDevice*                 GetDevice()                     { return m_pD3DDevice; }
    ID3DDeviceContext*          GetDeviceContext()              { return m_graphicsCommandList; }

    void                        OnResize(i32 _width, i32 _height);
    void                        SetClearColor(XCVec4& color)    { m_clearColor = color; }

    ID3DPipelineState*          GetPipelineState() { return m_pipelineState; }

    CPU_DESCRIPTOR_HANDLE       GetRTVCPUDescHandler();
    CPU_DESCRIPTOR_HANDLE       GetDSVCPUDescHandler();
    ID3DDepthStencilView*       GetDepthStencilView(RenderTargetsType type);

    void                        ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList);

protected:
    
    static void                 SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
    
    void                        CreateDescriptorHeaps();
    void                        CreateGraphicPipelineStateObjects();
    void                        SetupRenderTargets();
    void                        SetupDepthView();

    void                        DebugTestGraphicsPipeline();

    ID3D12Resource*             GetCurrentFrameRenderTarget() { return m_renderTarget[m_frameIndex]; }
    void                        WaitForPreviousFrameCompletion();

    void                        PresentRenderTarget(ID3D12GraphicsCommandList* cmdList);

private:
    IDXGISwapChain3*            m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC        m_SwapChainDesc;

    IDXGIFactory4*              m_pdxgiFactory;

    ID3DCommandAllocator*       m_pCommandAllocator;
    ID3D12CommandQueue*         m_pCommandQueue;
    ID3D12GraphicsCommandList*  m_graphicsCommandList;

    //RTV - 2 main render targets. Works in swaps
    ID3D12Resource*             m_renderTarget[2];
    ID3D12DescriptorHeap*       m_pRTVDescriptorHeap;
    i32                         m_rtvDescriptorSize;

    //DepthStencilResource
    ID3D12Resource*             m_depthStencilResource;
    ID3D12DescriptorHeap*       m_pDSVDescriptorHeap;

    //Constant buffer heap
    ID3D12DescriptorHeap*       m_constantBuffersHeap;

    //Fencing
    HANDLE                      m_fenceEvent;
    ID3D12Fence*                m_pFence;
    UINT64                      m_fenceValue;

    //FrameIndex
    u32                         m_frameIndex;

    std::atomic<i32>            m_cmdListRefCount;
    CriticalSection             m_acquireCmdListRefCount;

    ID3D12RootSignature*        m_rootSignature;
    ID3DPipelineState*          m_pipelineState;

#if defined(DEBUG_GRAPHICS_PIPELINE)
    // App resources.
    ID3D12Resource*             m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;
#endif
};

#endif  //if defined(XCGRAPHICS_DX12)