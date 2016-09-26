/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "SharedDescriptorHeap.h"

#include "Graphics/GPUResourceView.h"
#include "Graphics/GPUResource.h"

SharedDescriptorHeap::SharedDescriptorHeap()
#if defined(XCGRAPHICS_DX12)
    : m_rtvDescriptorSize(0)
#endif
{
    m_cs.Create(true, "SharedDescLock");
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
    dsvHeapDesc.NumDescriptors = nbOfDSVDesc;
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

GPUResource* SharedDescriptorHeap::AllocateGPUResource(GPUResourceType type, u32 sizeOfBuffer)
{
    GPUResource* outRes = nullptr;

    for each (GPUResource* res in m_gpuResources)
    {
        if (!res->IsInUse() && res->GetResourceType() == type && res->GetResourceSize() == sizeOfBuffer)
        {
            outRes = res;
            outRes->Destroy();
            outRes->SetInUse(true);
            break;
        }
    }

    if (outRes == nullptr)
    {
        //Create new
        outRes = XCNEW(GPUResource)(type, sizeOfBuffer);
        outRes->SetInUse(true);

        m_gpuResources.push_back(outRes);
    }

    return outRes;
}

GPUResourceView* SharedDescriptorHeap::AllocateGPUResourceView(GPUResourceType type)
{
    GPUResourceView* outView = nullptr;

    m_cs.Enter();

    for each (GPUResourceView* view in m_gpuResourcesView)
    {
        if (!view->IsInUse() && view->GetResourceType() == type)
        {
            outView = view;
            outView->Destroy();
            outView->SetInUse(true);
            break;
        }
    }

    if (outView == nullptr)
    {
        //Create new
        outView = XCNEW(GPUResourceView)(type);
        outView->SetInUse(true);

        m_gpuResourcesView.push_back(outView);
    }


    m_cs.Exit();

    return outView;
}

void SharedDescriptorHeap::DestroyGPUResource(GPUResource* resource)
{
    m_cs.Enter();

    resource->SetInUse(false);

    m_cs.Exit();
}

void SharedDescriptorHeap::Destroy()
{
    for each(GPUResource* res in m_gpuResources)
    {
        XCDELETE(res);
    }

    for each(GPUResourceView* view in m_gpuResourcesView)
    {
        XCDELETE(view);
    }

#if defined(XCGRAPHICS_DX12)
    for (u32 type = 0; type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++type)
    {
        XCDELETE(m_heapDesc[type]);
    }
#endif

    m_gpuResources.clear();
}