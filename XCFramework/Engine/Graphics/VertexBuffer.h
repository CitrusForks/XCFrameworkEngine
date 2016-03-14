/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"
#include "Engine/Graphics/IGraphicsBuffer.h"
#include "Engine/Utils/GraphicUtils.h"

template<class T>
class VertexBuffer : public IResource
{
public:

    VertexBuffer() 
    {
    }

    VertexBuffer(const VertexBuffer& vertexBuffer)
    {
        m_vertexData = vertexBuffer.m_vertexData;
    }

    ~VertexBuffer()
    {
    }

    void operator =(const VertexBuffer& vertexBuffer)
    {
        m_vertexData = vertexBuffer.m_vertexData;
    }

    void BuildVertexBuffer()
    {
        XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

#if defined(XCGRAPHICS_DX12)
        //Create VB
        int vbSize = sizeof(T) * m_vertexData.size();
        
        ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vbSize),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_pVertexBufferResource)));

        XCASSERT(m_pVertexBufferResource != nullptr);

        ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vbSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pVertexBufferUploadResource)));

        XCASSERT(m_pVertexBufferUploadResource != nullptr);

        RenderingPool& renderPool = graphicsSystem.GetRenderingPool();
        renderPool.RequestResourceDeviceContext(this);
        WaitResourceUpdate();

        //Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_pVertexBufferResource->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = vbSize;
        m_vertexBufferView.StrideInBytes = sizeof(T);
#elif defined(XCGRAPHICS_DX11)
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vbd.MiscFlags = 0;
        vbd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vInitData;

        vbd.ByteWidth = sizeof(T)* m_vertexData.size();
        vInitData.pSysMem = &m_vertexData[0];

        ValidateResult(graphicsSystem.GetDevice()->CreateBuffer(&vbd, &vInitData, &m_pVB));
#endif
    }

    void RenderContextCallback(RenderContext& renderContext)
    {
#if defined(XCGRAPHICS_DX12)
        ID3DDeviceContext& context = renderContext.GetDeviceContext();
        int vbSize = sizeof(T) * m_vertexData.size();

        D3D12_SUBRESOURCE_DATA vertexData = {};
        vertexData.pData = &m_vertexData[0];
        vertexData.RowPitch = vbSize;
        vertexData.SlicePitch = vbSize;

        UpdateSubresources<1>(&context, m_pVertexBufferResource, m_pVertexBufferUploadResource, 0, 0, 1, &vertexData);
        context.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBufferResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
#endif

        IResource::RenderContextCallback(renderContext);
    }

    void SetVertexBuffer(ID3DDeviceContext& context)
    {
#if defined(XCGRAPHICS_DX12)
        context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.IASetVertexBuffers(0, 1, &m_vertexBufferView);
#elif defined(XCGRAPHICS_DX11)
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        unsigned int stride = sizeof(T);
        unsigned int offset = 0;
        context.IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
#endif
    }

#if defined(XCGRAPHICS_DX12) 
    D3D12_VERTEX_BUFFER_VIEW            m_vertexBufferView;

    ID3DResource*                       m_pVertexBufferResource;
    ID3DResource*                       m_pVertexBufferUploadResource;
#elif defined(XCGRAPHICS_DX11)
    ID3D11Buffer*                       m_pVB;
#endif

    std::vector<T>                      m_vertexData;
};