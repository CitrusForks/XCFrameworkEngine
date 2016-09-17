/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "GPUResourceSystem.h"
#include "GPUResource.h"
#include "GPUResourceView.h"
#include "SharedDescriptorHeap.h"

void GPUResourceSystem::Init(ID3DDevice& device)
{
    m_device = &device;
}

GPUResource* GPUResourceSystem::CreateConstantBufferResource(GPUResourceDesc& desc)
{
    desc.m_bufferSize = (desc.m_bufferSize + 255) & ~255;     //Must be a multiple of 256

    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResource* resource = heap.AllocateGPUResource(desc.m_bufferType, desc.m_bufferSize);

    if (resource != nullptr)
    {
        XCASSERT(desc.m_bufferSize);

        //Create the cbv
        if (desc.m_bufferSize)
        {
            resource->SetResourceSize(desc.m_bufferSize);

#if defined(XCGRAPHICS_DX12)

            D3D12_HEAP_PROPERTIES heapDesc = {};
            heapDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapDesc.CreationNodeMask = 1;
            heapDesc.VisibleNodeMask = 1;

            ValidateResult(m_device->CreateCommittedResource(&heapDesc,
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(resource->GetResourceSize()),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&resource->GetPointerToResource<ID3DResource*>())));

            //Map the above heap with the actual cb on CPU. Keep it mapped.
            //TODO: Need to analyze whether the cpu mapped memory is released when cbResource is released. Check the dtor of D3DConstantBuffer.
            ValidateResult(resource->GetResource<ID3DResource*>()->Map(0, nullptr, reinterpret_cast<void**>(&resource->GetCPUMappedResource())));

#elif defined(XCGRAPHICS_DX11)

            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = desc.m_bufferSize;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            ValidateResult(m_device->CreateBuffer(&bd, nullptr, &resource->GetPointerToResource<ID3D11Buffer*>()));
#endif
        }
    }

    return resource;
}

void GPUResourceSystem::CreateConstantBufferView(GPUResource* res)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResourceView* resourceView = heap.AllocateGPUResourceView(GPUResourceType_CBV);

    if (resourceView != nullptr)
    {
        XCASSERT(res->GetResourceSize());

        //Create the cbv
        if (res->GetResourceSize())
        {
#if defined(XCGRAPHICS_DX12)

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc = {};
            cbDesc.BufferLocation = res->GetResource<ID3D12Resource*>()->GetGPUVirtualAddress();
            cbDesc.SizeInBytes = res->GetResourceSize();

            SharedDescriptorHeap::HeapDesc& desc = heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            m_device->CreateConstantBufferView(&cbDesc, desc.m_cbvCPUOffsetHandle);

            resourceView->SetCPUResourceViewHandle(desc.m_cbvCPUOffsetHandle);
            resourceView->SetGPUResourceViewHandle(desc.m_cbvGPUOffsetHandle);

            //Inc the offsets
            desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
            desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

#endif
        }
    }

    //Add the newly created view into the gpu resource
    res->AddResourceView(resourceView);
}

GPUResource* GPUResourceSystem::CreateConstantBufferResourceView(GPUResourceDesc& desc)
{
    GPUResource* outRes = CreateConstantBufferResource(desc);
    CreateConstantBufferView(outRes);

    return outRes;
}

GPUResource* GPUResourceSystem::CreateRenderTextureResource(ID3DSwapChain* swapChain, RenderTargetsType type)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResource* resource = heap.AllocateGPUResource(GPUResourceType_RTV, 0);

    if (resource != nullptr)
    {
#if defined(XCGRAPHICS_DX12)
        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        ValidateResult(swapChain->GetBuffer(type, IID_PPV_ARGS(&resource->GetPointerToResource<ID3DResource*>())));

#elif defined(XCGRAPHICS_DX11)
        swapChain->GetBuffer(type, __uuidof(ID3D11Texture2D), (void**) &resource->GetPointerToResource<ID3D11Texture2D*>());
#endif
    }

    return resource;
}

GPUResource* GPUResourceSystem::CreateTextureResource(D3D_TEXTURE2D_DESC& textureDesc, const D3D_SUBRESOURCE_DATA* subResourceData)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResource* resource = heap.AllocateGPUResource(GPUResourceType_SRV, 0);

    if (resource != nullptr)
    {
#if defined(XCGRAPHICS_DX12)

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        bool isRenderTarget = textureDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        ValidateResult(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &textureDesc,
            isRenderTarget ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&resource->GetPointerToResource<ID3DResource*>())));

#elif defined(XCGRAPHICS_DX11)
        ValidateResult(m_device->CreateTexture2D(&textureDesc, subResourceData, &resource->GetPointerToResource<ID3D11Texture2D*>()));
#endif
    }

    return resource;
}

void GPUResourceSystem::CreateShaderResourceView(D3D_SHADER_RESOURCE_VIEW_DESC& viewDesc, GPUResource* gpuResource)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResourceView* resourceView = heap.AllocateGPUResourceView(GPUResourceType_SRV);

    if (resourceView != nullptr)
    {
#if defined(XCGRAPHICS_DX12)
        SharedDescriptorHeap::HeapDesc& desc = heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        m_device->CreateShaderResourceView(gpuResource->GetResource<ID3D12Resource*>(), &viewDesc, desc.m_cbvCPUOffsetHandle);

        resourceView->SetCPUResourceViewHandle(desc.m_cbvCPUOffsetHandle);
        resourceView->SetGPUResourceViewHandle(desc.m_cbvGPUOffsetHandle);

        //Inc the offsets
        desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
        desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

#elif defined(XCGRAPHICS_DX11)
        // Create srv
        m_device->CreateShaderResourceView(gpuResource->GetResource<ID3D11Resource*>(), &viewDesc, &resourceView->GetPointerToResourceView<ID3D11ShaderResourceView*>());
#endif
    }
    else
    {
        XCASSERT(resourceView);
    }

    gpuResource->AddResourceView(resourceView);
}

void GPUResourceSystem::CreateRenderTargetView(GPUResource* gpuResource)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResourceView* resourceView = heap.AllocateGPUResourceView(GPUResourceType_RTV);

    if (resourceView != nullptr)
    {
#if defined(XCGRAPHICS_DX12)

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        ZeroMemory(&renderTargetViewDesc, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));

        //Extract the desc of texture
        D3D_TEXTURE2D_DESC textureDesc = gpuResource->GetResource<ID3DResource*>()->GetDesc();

        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        SharedDescriptorHeap::HeapDesc& desc = heap.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        m_device->CreateRenderTargetView(gpuResource->GetResource<ID3DResource*>(), &renderTargetViewDesc, desc.m_cbvCPUOffsetHandle);

        resourceView->SetCPUResourceViewHandle(desc.m_cbvCPUOffsetHandle);
        resourceView->SetGPUResourceViewHandle(desc.m_cbvGPUOffsetHandle);

        //Inc the offsets
        desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

#elif defined(XCGRAPHICS_DX11)
        //Extract the desc of texture
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

        D3D_TEXTURE2D_DESC textureDesc;
        gpuResource->GetResource<ID3D11Texture2D*>()->GetDesc(&textureDesc);

        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        ValidateResult(m_device->CreateRenderTargetView(gpuResource->GetResource<ID3DResource*>(), &renderTargetViewDesc, &resourceView->GetPointerToResourceView<ID3D11RenderTargetView*>()));
#endif
    }

    gpuResource->AddResourceView(resourceView);
}

#if defined(UNUSED)
void GPUResourceSystem::CreateRenderTargetView(D3D_TEXTURE2D_DESC& desc, GPUResource* gpuResource)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResourceView* resourceView = heap.AllocateGPUResourceView(GPUResourceType_RTV);

    if (resourceView != nullptr)
    {
#if defined(XCGRAPHICS_DX12)

        //Create Desc heap for shader specific.
        //Create descriptor heap for constant buffers
        D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        ZeroMemory(&renderTargetViewDesc, sizeof(D3D12_RENDER_TARGET_VIEW_DESC));

        renderTargetViewDesc.Format = desc.Format;
        renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        m_device->CreateRenderTargetView(gpuResource->GetResource<ID3D11Resource>(), &renderTargetViewDesc, m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->m_cbvCPUOffsetHandle);

        HeapDesc& desc = *m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        resourceView->m_cpuHandle = desc.m_cbvCPUOffsetHandle;
        resourceView->m_gpuHandle = desc.m_cbvGPUOffsetHandle;

        //Inc the offsets
        desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        desc.m_cbvGPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

#elif defined(XCGRAPHICS_DX11)

        // Create the render target texture.
        // Setup the description of the render target view.
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

        renderTargetViewDesc.Format = desc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        ValidateResult(m_device->CreateRenderTargetView(gpuResource->GetResource<ID3D11Resource>(), &renderTargetViewDesc, resourceView->GetResourceView<ID3D11RenderTargetView*>()));
#endif
    }

    gpuResource->AddResourceView(resourceView);
}
#endif

#if defined(UNUSED)
GPUResourceView* GPUResourceSystem::CreateRenderTargetView(ID3DSwapChain* swapChain, RenderTargetsType type)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    GPUResourceView* resourceView = heap.AllocateGPUResourceView(GPUResourceType_RTV);

    if (resourceView != nullptr)
    {
#if defined(XCGRAPHICS_DX12)

        m_device->CreateRenderTargetView(resourceView->m_cbResource, nullptr, m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->m_cbvCPUOffsetHandle);

        HeapDesc& desc = *m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        resourceView->m_cpuHandle = desc.m_cbvCPUOffsetHandle;

        //Inc the offsets
        desc.m_cbvCPUOffsetHandle.Offset(m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

#elif defined(XCGRAPHICS_DX11)
        ID3DResource* backBufferTexture;
        swapChain->GetBuffer(type, __uuidof(ID3DResource), (void**)resourceView->GetResource<ID3D11RenderTargetView*>());

        resourceView->m_cbResource = backBufferTexture;

        ID3D11RenderTargetView* renderTargetView = static_cast<ID3D11RenderTargetView*>(resourceView->m_resourceView);
        ValidateResult(m_device->CreateRenderTargetView(backBufferTexture, 0, &renderTargetView));

        ReleaseCOM(backBufferTexture);
#endif
    }

    return resourceView;
}
#endif

void GPUResourceSystem::DestroyResource(GPUResource* resource)
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyGPUResource(resource);
}

void GPUResourceSystem::Destroy()
{
}