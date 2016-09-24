/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)

#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

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

    u32                         GetCurrentRTVFrameIndex() { return m_frameIndex; }
    ID3DPipelineState*          GetPipelineState() { return m_pipelineState; }

    void                        SetRenderableTargetsContiguous(ID3DDeviceContext& context, std::vector<RenderTargetsType>& types, ID3DDepthStencilView* depthView);
    void                        ClearRTVAndDSV(ID3DDeviceContext* context, RenderTargetsType type);

protected:
    
    static void                 SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
    
    void                        CreateDescriptorHeaps();
    void                        CreateGPUResourceSystem();

    void                        SetupRenderTargets();
    void                        SetupDepthView();
    void                        SetupRenderQuad();

    void                        DebugTestGraphicsPipeline();

    ID3D12Resource*             GetCurrentFrameRenderTarget() { return m_renderTargets[m_frameIndex]->GetRenderTargetResource()->GetResource<ID3D12Resource*>(); }
    void                        WaitForPreviousFrameCompletion();

private:
    ID3DSwapChain*              m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC        m_SwapChainDesc;

    IDXGIFactory4*              m_pdxgiFactory;

    ID3DCommandAllocator*       m_pCommandAllocator;
    ID3D12CommandQueue*         m_pCommandQueue;
    ID3D12GraphicsCommandList*  m_graphicsCommandList;

    //Fencing
    HANDLE                      m_fenceEvent;
    ID3D12Fence*                m_pFence;
    UINT64                      m_fenceValue;

    //FrameIndex
    u32                         m_frameIndex;

    ID3D12RootSignature*        m_rootSignature;
    ID3DPipelineState*          m_pipelineState;


    VertexBuffer<VertexPosTex>* m_renderQuadVB;
    IndexBuffer<u32>*           m_renderQuadIB;

    std::atomic<i32>            m_cmdListRefCount;
    CriticalSection             m_acquireCmdListRefCount;

#if defined(DEBUG_GRAPHICS_PIPELINE)
    // App resources.
    ID3D12Resource*             m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW    m_vertexBufferView;
#endif
};

#endif  //if defined(XCGRAPHICS_DX12)