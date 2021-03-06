/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)

#include "Graphics/XCGraphics.h"
#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

#include "Base/Thread/CriticalSection.h"

class XCGraphicsDx12 : public XCGraphics
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XCGraphicsDx12)

#if defined(DEBUG_GRAPHICS_PIPELINE)
    struct Vertex
    {
        XCVec3 position;
        XCVec4 color;
    };
#endif

    XCGraphicsDx12(void);
    ~XCGraphicsDx12(void);
    
    void                        Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    void                        BeginScene();
    void                        EndScene();
    void                        Update(f32 dt);
    void                        Destroy();

    ID3DDevice*                 GetDevice()                     { return m_pD3DDevice; }
    ID3DDeviceContext*          GetDeviceContext()              { return m_pD3DDeviceContext; }

    void                        OnResize(i32 _width, i32 _height);

    ID3DPipelineState*          GetPipelineState() { return m_pipelineState; }

    void                        SetRenderableTargets(ID3DDeviceContext& context, const std::vector<RenderTargetsType>& types) override;
    void                        ClearRTVAndDSVs(ID3DDeviceContext& context, std::vector<RenderTargetsType>& type, XCVec4& clearColor) override;

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

    //Fencing
    HANDLE                      m_fenceEvent;
    ID3D12Fence*                m_pFence;
    UINT64                      m_fenceValue;

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