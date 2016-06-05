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

    struct Vertex
    {
        XCVec3 position;
        XCVec4 color;
    };

    XC_GraphicsDx12(void);
    ~XC_GraphicsDx12(void);
    
    void                        InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa);
    IDXGISwapChain*             GetSwapChain()      { return m_pSwapChain;   }
    
    ID3DDevice*                 GetDevice()                     { return m_pD3DDevice; }
    ID3DDeviceContext*          GetDeviceContext()              { return m_graphicsCommandList; }
    
    ID3DCommandAllocator*       GetDeviceCommandAllocator()     { return m_pCommandAllocator; }
    ID3DCommandQueue*           GetCommandQueue()               { return m_pCommandQueue; }

    void                        Update(float dt);
    
    void                        BeginScene();
    void                        EndScene();
    
    void                        OnResize(int _width, int _height);
    void                        SetClearColor(XCVec4 color)           { m_clearColor = color; }

    void                        CreateDescriptorHeaps();
    void                        CreateGraphicPipelineStateObjects();
    void                        SetupRenderTargets();
    void                        SetupDepthView();

    void                        DebugTestGraphicsPipeline();
    ID3D12PipelineState*        GetPipelineState() { return m_pipelineState; }

    ID3D12Resource*             GetCurrentFrameRenderTarget() { return m_renderTarget[m_frameIndex]; }
    CPU_DESCRIPTOR_HANDLE       GetRTVCPUDescHandler();
    CPU_DESCRIPTOR_HANDLE       GetDSVCPUDescHandler();

    void                        PresentRenderTarget(ID3D12GraphicsCommandList* cmdList);
    void                        ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList);

    static void                 SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
    void                        WaitForPreviousFrameCompletion();

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
    int                         m_rtvDescriptorSize;

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
    unsigned int                m_frameIndex;

    std::atomic<int>            m_cmdListRefCount;
    CriticalSection             m_acquireCmdListRefCount;

    ID3D12RootSignature*        m_rootSignature;
    ID3D12PipelineState*        m_pipelineState;

#if defined(DEBUG_GRAPHICS_PIPELINE)
    // App resources.
    ID3D12Resource*             m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;
#endif
};

#endif  //if defined(XCGRAPHICS_DX12)