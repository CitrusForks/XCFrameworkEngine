/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"
#include "Graphics/IGraphicsBuffer.h"
#include "Graphics/Utils/GraphicUtils.h"

template<class T>
class IndexBuffer : public IResource
{
public:

    IndexBuffer()
        :
#if defined(XCGRAPHICS_DX11)
          m_pIB(nullptr)
#elif defined(XCGRAPHICS_DX12)
          m_pIndexBufferResource(nullptr)
        , m_pIndexBufferUploadResource(nullptr)
#endif
    {
        m_resourceType = RESOURCETYPE_INDEXBUFFER;
    }

    ~IndexBuffer()
    {
#if defined(XCGRAPHICS_DX11)
        ReleaseCOM(m_pIB);
#elif defined(XCGRAPHICS_DX12)
        ReleaseCOM(m_pIndexBufferResource);
        ReleaseCOM(m_pIndexBufferUploadResource);
#endif
    }

    void UpdateState() override;

    void AddIndicesVA(std::initializer_list<T> indexList)
    {
        for (auto index : indexList)
        {
            m_indexData.push_back(index);
        }
    }

    //If context is passed, then this buffer won't wait for render context callback from render pool.
    void BuildIndexBuffer(ID3DDeviceContext* context = nullptr);
    void RenderContextCallback(ID3DDeviceContext& renderContext);

#if defined(XCGRAPHICS_DX12)
    D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_indexBufferView; }
#endif

    void* GetIndexBufferInGPUMem()      
    {
        return nullptr;
    }

#if defined(XCGRAPHICS_DX12)
    D3D12_INDEX_BUFFER_VIEW             m_indexBufferView;
    ID3DResource*                       m_pIndexBufferResource;
    ID3DResource*                       m_pIndexBufferUploadResource;
#elif defined(XCGRAPHICS_DX11)
    ID3D11Buffer*                       m_pIB;
#endif

    std::vector<T>                      m_indexData;
};


template<class T>
void IndexBuffer<T>::BuildIndexBuffer(ID3DDeviceContext* context)
{
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

#if defined(XCGRAPHICS_DX12)
    //Create IB
    i32 ibSize = sizeof(u32) * m_indexData.size();

    ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(ibSize),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_pIndexBufferResource)));

    ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(ibSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pIndexBufferUploadResource)));

    if (context != nullptr)
    {
        RenderContextCallback(*context);
    }
    else
    {
        RenderingPool& renderPool = graphicsSystem.GetRenderingPool();
        renderPool.RequestResourceDeviceContext(this);
        WaitResourceUpdate();
    }

    //Initialize the vertex buffer view.
    m_indexBufferView.BufferLocation = m_pIndexBufferResource->GetGPUVirtualAddress();
    m_indexBufferView.SizeInBytes = ibSize;
    m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
#elif defined(XCGRAPHICS_DX11)
    D3D11_BUFFER_DESC ibd;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(T) * m_indexData.size();
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iInitData;
    iInitData.pSysMem = &m_indexData[0];

    ValidateResult(graphicsSystem.GetDevice()->CreateBuffer(&ibd, &iInitData, &m_pIB));

    m_resourceUpdated = true;
#endif
}

template<class T>
void IndexBuffer<T>::RenderContextCallback(ID3DDeviceContext& context)
{
#if defined(XCGRAPHICS_DX12)
    i32 ibSize = sizeof(u32) * m_indexData.size();

    D3D12_SUBRESOURCE_DATA indexData = {};
    indexData.pData = &m_indexData[0];
    indexData.RowPitch = ibSize;
    indexData.SlicePitch = ibSize;

    UpdateSubresources<1>(&context, m_pIndexBufferResource, m_pIndexBufferUploadResource, 0, 0, 1, &indexData);
    context.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBufferResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
#endif

    IResource::RenderContextCallback(context);

    m_resourceUpdated = true;
}

template<class T>
void IndexBuffer<T>::UpdateState()
{
    if (m_resourceUpdated)
    {
        m_resourceState = IResource::ResourceState_Loaded;
    }

    IResource::UpdateState();
}