/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "GPUResource.h"

GPUResource::~GPUResource()
{
    Destroy();
}

void GPUResource::Destroy()
{
    for (GPUResourceView* view : m_resourceViews)
    {
        view->Destroy();
    }

    //Destroy the views
    if (m_cpuMappedResource != nullptr)
    {
        //Dx12 : Not sure of this. Because the m_gpuResource gpu memory is mapped on cpu and is controlled by dx. Anyway if at all the memory is not deallocated, it will be.
        delete m_cpuMappedResource;
    }

    ReleaseCOM(m_gpuResource);
}

void GPUResource::AddResourceView(GPUResourceView* view)
{
    m_resourceViews.push_back(view);
}

GPUResourceView* GPUResource::GetResourceView(GPUResourceType type)
{
    auto retView = std::find_if(m_resourceViews.begin(), m_resourceViews.end(), 
        [&type](GPUResourceView* view) -> bool 
        {
             return view->GetResourceType() == type;
        });

    XCASSERT(retView != m_resourceViews.end());

    return (*retView);
}

void GPUResource::SetInUse(bool inUse)
{
    m_isInUse = inUse;

    for(GPUResourceView* view : m_resourceViews)
    {
        view->SetInUse(m_isInUse);
    }
}

void GPUResource::UploadZeroMemoryDataOnGPU(ID3DDeviceContext& context, u32 sizeOfBuffer)
{
    XCASSERT(m_sizeOfBuffer >= sizeOfBuffer);

#if defined(XCGRAPHICS_DX12)
    memset(m_cpuMappedResource, 0, sizeOfBuffer);
#elif defined(XCGRAPHICS_DX11)
    /*
    //TODO: Map, copy and unmap. This could be called from other threads which is asserted by d3d11. So disabled currently.
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(context.Map(m_gpuHandle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memset(mappedData.pData, 0, sizeOfBuffer);
    context.Unmap(m_gpuHandle, 0);
    */
#endif
}

void GPUResource::UploadDataOnGPU(ID3DDeviceContext& context, void* buffer, u32 sizeOfBuffer)
{
    XCASSERT(m_sizeOfBuffer >= sizeOfBuffer);

#if defined(XCGRAPHICS_DX12)
    memcpy(m_cpuMappedResource, buffer, sizeOfBuffer);
#elif defined(XCGRAPHICS_DX11)
    //Map, copy and unmap
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(context.Map(m_gpuResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memcpy(mappedData.pData, buffer, sizeOfBuffer);
    context.Unmap(m_gpuResource, 0);
#endif
}