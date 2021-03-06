/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/RenderContext.h"
#include "Graphics/XCGraphics.h"
#include "Graphics/XCShaders/XCShaderContainer.h"
#include "Graphics/XCShaders/IShader.h"
#include "Graphics/SharedDescriptorHeap.h"

RenderContext::RenderContext()
{
}

void RenderContext::Init(XCShaderContainer* shaderMgr)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_shaderContainer = shaderMgr;

    m_graphicsSystem = &SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    ID3DDevice* device = m_graphicsSystem->GetDevice();

#if defined(USE_IMMEDIATE_CONTEXT)
    m_deviceContext = m_graphicsSystem->GetDeviceContext();
#else
    #if defined(XCGRAPHICS_DX12)
        ValidateResult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    
        ValidateResult(device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator,
            m_shaderContainer->GetShader(ShaderType_SolidColor)->GetPso().m_psos[RasterType_FillSolid].m_pPso,
            IID_PPV_ARGS(&m_deviceContext)));
    
        ValidateResult(m_deviceContext->Close());
    #elif defined(XCGRAPHICS_DX11)
        device->CreateDeferredContext(0, &m_deviceContext);
    #endif
#endif

}

void RenderContext::Destroy()
{
#if defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_DX12)
    ReleaseCOM(m_deviceContext);
#endif
}

void RenderContext::Reset()
{
#if defined(XCGRAPHICS_DX12)

    ValidateResult(m_commandAllocator->Reset());
    ValidateResult(m_deviceContext->Reset(m_commandAllocator, m_graphicsSystem->GetPipelineState()));
#elif defined(XCGRAPHICS_DX11)
    m_deviceContext->ClearState();
#endif
}

void RenderContext::BeginRender(std::vector<RenderTargetsType>& targetType)
{
    D3D_VIEWPORT& viewPort = m_graphicsSystem->GetViewPort(targetType.front());
    XCASSERT(viewPort.Width > 0 && viewPort.Height > 0);

#if defined(USE_IMMEDIATE_CONTEXT)

#else
    #if defined(XCGRAPHICS_DX12)

        m_deviceContext->RSSetViewports(1, &viewPort);
        m_deviceContext->RSSetScissorRects(1, &m_graphicsSystem->GetScissorRect());

        m_graphicsSystem->SetRenderableTargets(*m_deviceContext, targetType);

        //Set descriptor heaps
        SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
        ID3D12DescriptorHeap* ppHeaps[] = { heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).m_heapDesc, 
            heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).m_heapDesc };

        m_deviceContext->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    #elif defined(XCGRAPHICS_DX11)
    
        m_deviceContext->RSSetViewports(1, &m_graphicsSystem->GetViewPort(targetType.front()));

        m_graphicsSystem->SetRenderableTargets(*m_deviceContext, targetType);
    
    #elif defined(XCGRAPHICS_GNM)
        m_graphicsSystem->GetRenderTexture(targetType).SetRenderableTarget(m_deviceContext, nullptr);
    #endif
#endif
}

void RenderContext::FinishRender()
{
#if defined(USE_IMMEDIATE_CONTEXT)

#else
    #if defined(XCGRAPHICS_DX12)
        //Cmdlist Close can happen here when in multiple render context.
        ValidateResult(m_deviceContext->Close());
    #elif defined(XCGRAPHICS_DX11)
        ID3DCommandList* commandList;
        m_deviceContext->FinishCommandList(false, &commandList);

        //Execute on main device context
        m_graphicsSystem->GetDeviceContext()->ExecuteCommandList(commandList, true);
        ReleaseCOM(commandList);

        m_shaderContainer->ClearShaderAndRenderStates(*m_deviceContext);
    #endif
#endif
}

void RenderContext::ApplyShader(ShaderType shaderType, RasterType type)
{
    m_shaderContainer->ApplyShader(*m_deviceContext, shaderType, type);
}

IShader* RenderContext::GetShader(ShaderType shaderType)
{
    return m_shaderContainer->GetShader(shaderType);
}

void RenderContext::DrawNonIndexed(u32 vertexCount)
{
#if defined(XCGRAPHICS_DX12)
    m_deviceContext->DrawInstanced(vertexCount, 1, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    m_deviceContext->Draw(vertexCount, 0);
#endif
}

void RenderContext::DrawIndexedInstanced(u32 _indexCount, void* indexGpuAddr, u32 instanceCount)
{
    m_deviceContext->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}

GlobalShaderData& RenderContext::GetGlobalShaderData()
{
    return m_shaderContainer->GetGlobalShaderData();
}