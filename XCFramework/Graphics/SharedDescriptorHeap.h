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

    //TODO: Get count of FBTexture2DS from data
    static const i32        nbOfTexturesFromData = 14;

    SharedDescriptorHeap();
    ~SharedDescriptorHeap()    {}

    void                    Init(ID3DDevice& device, u32 nbOfDesc);
    D3DConstantBuffer*      CreateBufferView(D3DBufferDesc& desc);

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*      CreateShaderResourceView(CD3DX12_RESOURCE_DESC& textureDesc, D3D12_SHADER_RESOURCE_VIEW_DESC& viewDesc);
    ID3D12DescriptorHeap*   GetDescriptorHeap() { return m_sharedDescriptorHeap; }
#elif defined(XCGRAPHICS_DX11)
    D3DConstantBuffer*      CreateShaderResourceView();
#endif

    void                    DestroyBuffer(D3DConstantBuffer* buffer);
    void                    Destroy();

private:

    D3DConstantBuffer*      FindFreeConstantBuffer(BufferType type, u32 size);

#if defined(XCGRAPHICS_DX12)
    ID3D12DescriptorHeap*           m_sharedDescriptorHeap; //SRV, CB, UAV only
    CD3DX12_CPU_DESCRIPTOR_HANDLE   m_cbvCPUOffsetHandle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE   m_cbvGPUOffsetHandle;
#endif

    ID3DDevice*                     m_device;
    std::vector<D3DConstantBuffer*> m_constantBuffers;
};