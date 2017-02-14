/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

#include "Graphics/GPUResourceType.h"
#include "Graphics/RenderTargetTypes.h"
#include "Base/Thread/CriticalSection.h"

class GPUResource;
class GPUResourceView;

class SharedDescriptorHeap : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(SharedDescriptorHeap)

#if defined(XCGRAPHICS_DX12)
    struct HeapDesc
    {
        explicit HeapDesc(D3D12_DESCRIPTOR_HEAP_TYPE type)
            : m_type(type)
            , m_heapDesc(nullptr)
        {}

        ~HeapDesc()
        {
            ReleaseCOM(m_heapDesc);
        }

        D3D12_DESCRIPTOR_HEAP_TYPE      m_type;
        CD3DX12_CPU_DESCRIPTOR_HANDLE   m_cbvCPUOffsetHandle;
        CD3DX12_GPU_DESCRIPTOR_HANDLE   m_cbvGPUOffsetHandle;
        ID3D12DescriptorHeap*           m_heapDesc;
    };
#endif

    //TODO: Get count of FBTexture2DS from data
    static const i32                nbOfTexturesFromData = 14;

    SharedDescriptorHeap();
    ~SharedDescriptorHeap()    {}

    void                            Init(u32 nbOfRTVDesc, u32 nbOfDSVDesc, u32 nbOfSamplerDesc, u32 nbOfCBVDesc);

#if defined(XCGRAPHICS_DX12)
    HeapDesc&                       GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return *m_heapDesc[type]; }
    u32                             GetRTVDescHeapIncSize() { return m_rtvDescriptorSize; }
#endif

    GPUResource*                    AllocateGPUResource(GPUResourceType type, u32 sizeOfBuffer);
    GPUResourceView*                AllocateGPUResourceView(GPUResourceType type);

    void                            DestroyGPUResource(GPUResource* resource);
    void                            Destroy();

protected:
#if defined(XCGRAPHICS_DX12)
    HeapDesc*                       CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_DESC desc);
#endif

private:


#if defined(XCGRAPHICS_DX12)
    HeapDesc*                       m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    u32                             m_rtvDescriptorSize;
#endif

    std::vector<GPUResource*>       m_gpuResources;
    std::vector<GPUResourceView*>   m_gpuResourcesView;
    CriticalSection                 m_cs;
};