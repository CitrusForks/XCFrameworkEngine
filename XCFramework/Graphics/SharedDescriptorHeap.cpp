/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "SharedDescriptorHeap.h"


SharedDescriptorHeap::SharedDescriptorHeap()
#if defined(XCGRAPHICS_DX12)
    : m_rtvDescriptorSize(0)
#endif
{
}

void SharedDescriptorHeap::Init(ID3DDevice& device, u32 nbOfRTVDesc, u32 nbOfDSVDesc, u32 nbOfSamplerDesc, u32 nbOfCBVDesc)
{
    m_device = &device;

#if defined(XCGRAPHICS_DX12)
    //Create render targets. Acts like swapping render targets.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = nbOfRTVDesc;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = CreateHeapDesc(rtvHeapDesc);
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //Create descriptor heap for Depth Shader View
    //Allocate on heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = CreateHeapDesc(dsvHeapDesc);

    //Create descriptor heap for sampler
    D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
    samplerHeapDesc.NumDescriptors = nbOfSamplerDesc;
    samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = CreateHeapDesc(samplerHeapDesc);

    //Create CBV/UAV descripto heap.
    D3D12_DESCRIPTOR_HEAP_DESC cbHeapDesc = {};
    cbHeapDesc.NumDescriptors = nbOfCBVDesc;
    cbHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = CreateHeapDesc(cbHeapDesc);
#endif
}

#if defined(XCGRAPHICS_DX12)
SharedDescriptorHeap::HeapDesc* SharedDescriptorHeap::CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_DESC heapDesc)
{
    HeapDesc* desc = XCNEW(HeapDesc)(heapDesc.Type);
    ValidateResult(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&desc->m_heapDesc)));

    desc->m_cbvCPUOffsetHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(desc->m_heapDesc->GetCPUDescriptorHandleForHeapStart());
    desc->m_cbvGPUOffsetHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(desc->m_heapDesc->GetGPUDescriptorHandleForHeapStart());

    return desc;
}
#endif

D3DConstantBuffer* SharedDescriptorHeap::CreateBufferView(D3DBufferDesc& desc)
{
    desc.m_bufferSize = (desc.m_bufferSize + 255) & ~255;     //Must be a multiple of 256

    D3DConstantBuffer* constBuffer = FindFreeConstantBuffer(desc.m_bufferType, desc.m_bufferSize);

    if (constBuffer != nullptr)
    {
        constBuffer->m_isInUse = true;
        return constBuffer;
    }
    else
    {
        XCASSERT(desc.m_bufferSize);

        constBuffer = XCNEW(D3DConstantBuffer)(BUFFERTYPE_CBV);

        //Create the cbv
        if (desc.m_bufferSize)
        {
            constBuffer->m_sizeOfBuffer = desc.m_bufferSize;
            constBuffer->m_isInUse      = true;

#if defined(XCGRAPHICS_DX12)

            D3D12_HEAP_PROPERTIES heapDesc = {};
            heapDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapDesc.CreationNodeMask = 1;
            heapDesc.VisibleNodeMask = 1;

            ValidateResult(m_device->CreateCommittedResource(&heapDesc,
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(constBuffer->m_sizeOfBuffer),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&constBuffer->m_cbResource)));

            //Map the above heap with the actual cb on CPU. Keep it mapped.
            //TODO: Need to analyze whether the cpu mapped memory is released when cbResource is released. Check the dtor of D3DConstantBuffer.
            ValidateResult(constBuffer->m_cbResource->Map(0, nullptr, reinterpret_cast<void**>(&constBuffer->m_cbDataBegin)));

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc = {};
            cbDesc.BufferLocation = constBuffer->m_cbResource->GetGPUVirtualAddress();
            cbDesc.SizeInBytes = constBuffer->m_sizeOfBuffer;

            m_device->CreateConstantBufferView(&cbDesc, m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->m_cbvCPUOffsetHandle);

            HeapDesc& desc = *m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
            constBuffer->m_cpuHandle = desc.m_cbvCPUOffsetHandle;
            constBuffer->m_gpuHandle = desc.m_cbvGPUOffsetHandle;

            //Inc the offsets
            desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

#elif defined(XCGRAPHICS_DX11)
            
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = desc.m_bufferSize;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            ValidateResult(m_device->CreateBuffer(&bd, nullptr, &constBuffer->m_gpuHandle));

#endif
            m_constantBuffers.push_back(constBuffer);

            return m_constantBuffers.back();
        }
        else
        {
            XCDELETE(constBuffer);
        }
    }

    XCASSERT(false);
    return nullptr;
}

#if defined(XCGRAPHICS_DX12)
D3DConstantBuffer* SharedDescriptorHeap::CreateShaderResourceView(CD3DX12_RESOURCE_DESC& textureDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& viewDesc)
#elif defined(XCGRAPHICS_DX11)
D3DConstantBuffer* SharedDescriptorHeap::CreateShaderResourceView()
#endif
{
    D3DConstantBuffer* constBuffer = FindFreeConstantBuffer(BUFFERTYPE_SRV, 0);

    if (constBuffer != nullptr)
    {
        constBuffer->m_isInUse = true;
        return constBuffer;
    }
    else
    {
        constBuffer = XCNEW(D3DConstantBuffer)(BUFFERTYPE_SRV);
        constBuffer->m_sizeOfBuffer = 0;
        constBuffer->m_isInUse = true;

#if defined(XCGRAPHICS_DX12)

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        ValidateResult(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&constBuffer->m_cbResource)));

        m_device->CreateShaderResourceView(constBuffer->m_cbResource, &viewDesc, m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->m_cbvCPUOffsetHandle);

        HeapDesc& desc = *m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        constBuffer->m_cpuHandle = desc.m_cbvCPUOffsetHandle;
        constBuffer->m_gpuHandle = desc.m_cbvGPUOffsetHandle;

        //Inc the offsets
        desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

#endif
        m_constantBuffers.push_back(constBuffer);

        return m_constantBuffers.back();
    }
}

D3DConstantBuffer* SharedDescriptorHeap::FindFreeConstantBuffer(BufferType type, u32 size)
{
    for each (D3DConstantBuffer* buffer in m_constantBuffers)
    {
        if (!buffer->m_isInUse && buffer->m_bufferType == type && (type == BUFFERTYPE_SRV || buffer->m_sizeOfBuffer == size))
        {
            return buffer;
        }
    }
    return nullptr;
}

void SharedDescriptorHeap::DestroyBuffer(D3DConstantBuffer* buffer)
{
    buffer->m_isInUse = false;
}

void SharedDescriptorHeap::Destroy()
{
    for each(D3DConstantBuffer* buffer in m_constantBuffers)
    {
        XCDELETE(buffer);
    }

#if defined(XCGRAPHICS_DX12)
    for (u32 type = 0; type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++type)
    {
        XCDELETE(m_heapDesc[type]);
    }
#endif

    m_constantBuffers.clear();
}