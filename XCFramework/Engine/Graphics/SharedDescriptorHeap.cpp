/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "SharedDescriptorHeap.h"

#if defined(XCGRAPHICS_DX12)

SharedDescriptorHeap::SharedDescriptorHeap()
{
}

void SharedDescriptorHeap::Init(ID3DDevice& device, unsigned int nbOfDesc)
{
    m_device = &device;

    D3D12_DESCRIPTOR_HEAP_DESC cbHeapDesc = {};
    cbHeapDesc.NumDescriptors = nbOfDesc;
    cbHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ValidateResult(m_device->CreateDescriptorHeap(&cbHeapDesc, IID_PPV_ARGS(&m_sharedDescriptorHeap)));

    m_cbvCPUOffsetHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_sharedDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    m_cbvGPUOffsetHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_sharedDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

D3DConstantBuffer* SharedDescriptorHeap::CreateBufferView(D3DBufferDesc& desc)
{
    desc.m_bufferSize = (desc.m_bufferSize + 255) & ~255;     //Must be a multiple of 256

    D3DConstantBuffer* constBuffer = findFreeConstantBuffer(desc.m_bufferType, desc.m_bufferSize);

    if (constBuffer != nullptr)
    {
        constBuffer->m_isInUse = true;
        return constBuffer;
    }
    else
    {
        constBuffer = new D3DConstantBuffer;

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers

        XCASSERT(desc.m_bufferSize);
        //Create the cbv
        if (desc.m_bufferSize)
        {
            constBuffer->m_bufferType = BUFFERTYPE_CBV;
            constBuffer->m_sizeOfBuffer = desc.m_bufferSize;

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
            ValidateResult(constBuffer->m_cbResource->Map(0, nullptr, reinterpret_cast<void**>(&constBuffer->m_cbDataBegin)));

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc = {};
            cbDesc.BufferLocation = constBuffer->m_cbResource->GetGPUVirtualAddress();
            cbDesc.SizeInBytes = constBuffer->m_sizeOfBuffer;

            D3DConstantBuffer constBuffer(BUFFERTYPE_CBV);
            m_device->CreateConstantBufferView(&cbDesc, m_cbvCPUOffsetHandle);
        }

        constBuffer->m_cpuHandle = m_cbvCPUOffsetHandle;
        constBuffer->m_gpuHandle = m_cbvGPUOffsetHandle;
        constBuffer->m_isInUse = true;

        //Inc the offsets
        m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

        m_constantBuffers.push_back(constBuffer);

        return m_constantBuffers.back();
    }
}

D3DConstantBuffer* SharedDescriptorHeap::CreateShaderResourceView(CD3DX12_RESOURCE_DESC& textureDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& viewDesc)
{
    D3DConstantBuffer* constBuffer = findFreeConstantBuffer(BUFFERTYPE_SRV, 0);

    if (constBuffer != nullptr)
    {
        constBuffer->m_isInUse = true;
        return constBuffer;
    }
    else
    {
        constBuffer = new D3DConstantBuffer;
        constBuffer->m_bufferType = BUFFERTYPE_SRV;
        constBuffer->m_sizeOfBuffer = 0;

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        ValidateResult(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&constBuffer->m_cbResource)));

        m_device->CreateShaderResourceView(constBuffer->m_cbResource, &viewDesc, m_cbvCPUOffsetHandle);

        constBuffer->m_cpuHandle = m_cbvCPUOffsetHandle;
        constBuffer->m_gpuHandle = m_cbvGPUOffsetHandle;
        constBuffer->m_isInUse = true;

        //Inc the offsets
        m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

        m_constantBuffers.push_back(constBuffer);

        return m_constantBuffers.back();
    }
}


D3DConstantBuffer* SharedDescriptorHeap::findFreeConstantBuffer(BufferType type, unsigned int size)
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
        delete(buffer);
    }

    m_constantBuffers.clear();
}

#endif