/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

#include "GPUResourceType.h"

class GPUResourceView
{
public:
    explicit GPUResourceView(GPUResourceType type);
    ~GPUResourceView();

    void Destroy();

    void SetInUse(bool inUse) { m_isInUse = inUse; }
    bool IsInUse() { return m_isInUse; }

    GPUResourceType GetResourceType() { return m_resourceType; }

    void SetGPUResourceViewHandle(const GPU_DESCRIPTOR_HANDLE& desc)   { m_gpuHandle = desc; }
    void SetCPUResourceViewHandle(const CPU_DESCRIPTOR_HANDLE& desc)   { m_cpuHandle = desc; }
    
    const GPU_DESCRIPTOR_HANDLE& GetGPUResourceViewHandle()            { return m_gpuHandle; }
    const CPU_DESCRIPTOR_HANDLE& GetCPUResourceViewHandle()            { return m_cpuHandle; }

    template<class T>
    T GetResourceView()
    {
        XCASSERT(m_resourceType != GPUResourceType_CBV);
        return (T) m_resourceView;
    }

    template<class T>
    T& GetPointerToResourceView()
    {
        XCASSERT(m_resourceType != GPUResourceType_CBV);
        return (T&)m_resourceView;
    }

    bool IsValid()
    {
#if defined(XCGRAPHICS_DX12)
        return m_cpuHandle.ptr != 0;
#elif defined(XC_GRAPHICSDX11)
        return m_resourceView != nullptr;
#endif
    }

private:
    GPU_DESCRIPTOR_HANDLE        m_gpuHandle;
    CPU_DESCRIPTOR_HANDLE        m_cpuHandle;

    ID3DResourceView*            m_resourceView;
    
    GPUResourceType              m_resourceType;
    bool                         m_isInUse;
};