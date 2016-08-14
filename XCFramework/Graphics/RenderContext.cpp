/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/RenderContext.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Shaders/XC_ShaderContainer.h"
#include "Graphics/XC_Shaders/IShader.h"
#include "Graphics/SharedDescriptorHeap.h"

RenderContext::RenderContext()
{
}

void RenderContext::Init(ID3DDevice* device, XC_ShaderContainer* shaderMgr, bool clearStateOnBegin, bool clearStateOnFinish)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_shaderManager = shaderMgr;
    m_clearStateOnBegin = clearStateOnBegin;
    m_clearStateOnFinish = clearStateOnFinish;

    m_graphicsSystem = &SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

#if defined(USE_IMMEDIATE_CONTEXT)
    m_deviceContext = m_graphicsSystem->GetDeviceContext();
#else
    #if defined(XCGRAPHICS_DX12)
        ValidateResult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    
        ValidateResult(device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator,
            m_shaderManager->GetShader(ShaderType_SolidColor)->GetPso().m_psos[PSOType_RASTER_FILL_SOLID].m_pPso,
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

void RenderContext::BeginRender(RenderTargetsType targetType)
{
#if defined(USE_IMMEDIATE_CONTEXT)

#else
    #if defined(XCGRAPHICS_DX12)
        ValidateResult(m_commandAllocator->Reset());
        ValidateResult(m_deviceContext->Reset(m_commandAllocator, m_graphicsSystem->GetPipelineState()));
    
        m_deviceContext->RSSetViewports(1, &m_graphicsSystem->GetViewPort(RENDERTARGET_MAIN_0));
        m_deviceContext->RSSetScissorRects(1, &m_graphicsSystem->GetScissorRect());
    
        SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV).m_heapDesc->GetCPUDescriptorHandleForHeapStart());
        rtvHandle.ptr = rtvHandle.ptr + (m_graphicsSystem->GetCurrentRTVFrameIndex() * heap.GetRTVDescHeapIncSize());

        m_deviceContext->OMSetRenderTargets(1, &rtvHandle, false, &heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV).m_cbvCPUOffsetHandle);
    
        //Set descriptor heaps
        ID3D12DescriptorHeap* ppHeaps[] = { heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).m_heapDesc, 
            heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER).m_heapDesc };

        m_deviceContext->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    #elif defined(XCGRAPHICS_DX11)
        if (m_clearStateOnBegin)
        {
            m_deviceContext->ClearState();
        }
    
        m_deviceContext->RSSetViewports(1, &m_graphicsSystem->GetViewPort(targetType));
        m_graphicsSystem->GetRenderTexture(targetType).SetRenderableTarget(*m_deviceContext, m_graphicsSystem->GetDepthStencilView(targetType));
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
        m_deviceContext->FinishCommandList(m_clearStateOnFinish, &m_commandList);

        //Execute on main device context
        m_graphicsSystem->GetDeviceContext()->ExecuteCommandList(m_commandList, false);
        m_shaderManager->ClearShaderAndRenderStates(*m_deviceContext);

        ReleaseCommandList();
    #endif
#endif
}

void RenderContext::ReleaseCommandList()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_commandList);
#endif
}

void RenderContext::SetRasterizerState(RasterType type)
{
#if defined(XCGRAPHICS_DX11)
    m_shaderManager->SetRasterizerState(*m_deviceContext, type);
#endif
}

void RenderContext::ApplyShader(ShaderType shaderType)
{
    m_shaderManager->ApplyShader(*m_deviceContext, shaderType);
}

IShader* RenderContext::GetShader(ShaderType shaderType)
{
    return m_shaderManager->GetShader(shaderType);
}

void RenderContext::DrawNonIndexed(ID3DDeviceContext& context, u32 vertexCount)
{
#if defined(XCGRAPHICS_DX12)
    m_deviceContext->DrawInstanced(vertexCount, 1, 0, 0);
#elif defined(XCGRAPHICS_DX11)
    m_deviceContext->Draw(vertexCount, 0);
#endif
}

void RenderContext::DrawIndexedInstanced(ID3DDeviceContext& context, u32 _indexCount, void* indexGpuAddr, u32 instanceCount)
{
    m_deviceContext->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}

GlobalShaderData& RenderContext::GetGlobalShaderData()
{
    return m_shaderManager->GetGlobalShaderData();
}