/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

#include "Graphics/D3DConstantBuffer.h"

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
    static const i32        nbOfTexturesFromData = 14;

    SharedDescriptorHeap();
    ~SharedDescriptorHeap()    {}

    void                            Init(ID3DDevice& device, u32 nbOfRTVDesc, u32 nbOfDSVDesc, u32 nbOfSamplerDesc, u32 nbOfCBVDesc);
    D3DConstantBuffer*              CreateBufferView(D3DBufferDesc& desc);

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*              CreateShaderResourceView(CD3DX12_RESOURCE_DESC& textureDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& viewDesc);
    HeapDesc&                       GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) { return *m_heapDesc[type]; }
    u32                             GetRTVDescHeapIncSize() { return m_rtvDescriptorSize; }
#elif defined(XCGRAPHICS_DX11)
    D3DConstantBuffer*              CreateShaderResourceView();
#endif

    void                            DestroyBuffer(D3DConstantBuffer* buffer);
    void                            Destroy();

protected:
#if defined(XCGRAPHICS_DX12)
    HeapDesc*                       CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_DESC desc);
#endif

private:

    D3DConstantBuffer*              FindFreeConstantBuffer(BufferType type, u32 size);

#if defined(XCGRAPHICS_DX12)
    HeapDesc*                       m_heapDesc[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    u32                             m_rtvDescriptorSize;
#endif

    ID3DDevice*                     m_device;
    std::vector<D3DConstantBuffer*> m_constantBuffers;
};