/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "GPUResourceType.h"
#include "GPUResourceView.h"

class GPUResource
{
public:
    GPUResource(GPUResourceType type, u32 resSize)
        : m_gpuResource(nullptr)
        , m_cpuMappedResource(nullptr)
        , m_resourceType(type)
        , m_sizeOfBuffer(resSize)
    {}

    ~GPUResource();

    void Destroy();

    //Sets the resource in use and all its resource views too.
    void SetInUse(bool inUse);
    bool IsInUse() { return m_isInUse; }

    u32  GetResourceSize() { return m_sizeOfBuffer; }
    void SetResourceSize(u32 resSize) { m_sizeOfBuffer = resSize; }
    
    GPUResourceType GetResourceType() { return m_resourceType; }

    void UploadZeroMemoryDataOnGPU(ID3DDeviceContext& context, u32 sizeOfBuffer);
    void UploadDataOnGPU(ID3DDeviceContext& context, void* buffer, u32 sizeOfBuffer);

    void AddResourceView(GPUResourceView* view);



    template<class Typed>
    Typed           GetGPUResourceViewTyped(GPUResourceType type)
    {
        GPUResourceView* view = GetResourceView(type);
        return view->GetResourceView<Typed>();
    }

    template<class Typed>
    Typed&          GetPointerToGPUResourceViewTyped(GPUResourceType type)
    {
        GPUResourceView* view = GetResourceView(type);
        return view->GetPointerToResourceView<Typed>();
    }

    GPUResourceView* GetResourceView(GPUResourceType type);

    template<class T>
    T GetResource()
    {
        return (T) m_gpuResource;
    }

    template<class T>
    T& GetPointerToResource()
    {
        return (T&) m_gpuResource;
    }

    u32*& GetCPUMappedResource()
    {
        return (u32*&)m_cpuMappedResource;
    }

private:
    ID3DResource*                 m_gpuResource;
    u32*                          m_cpuMappedResource;
    GPUResourceType               m_resourceType;
    u32                           m_sizeOfBuffer;
    bool                          m_isInUse;

    std::vector<GPUResourceView*> m_resourceViews;
};