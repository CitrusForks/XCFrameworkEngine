/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

#include "Graphics/Utils/GraphicUtils.h"

enum BufferType
{
    BUFFERTYPE_SRV,
    BUFFERTYPE_CBV,
    BUFFERTYPE_UAV
};

class D3DBufferDesc
{
public:
    D3DBufferDesc(BufferType type, unsigned int size)
        : m_bufferType(type), m_bufferSize(size)
    {
    }

    BufferType   m_bufferType;
    unsigned int m_bufferSize;
};

class D3DConstantBuffer
{
public:

    explicit D3DConstantBuffer(BufferType type);

    void UploadZeroMemoryDataOnGPU(ID3DDeviceContext& context, unsigned int sizeOfBuffer);
    void UploadDataOnGPU(ID3DDeviceContext& context, void* buffer, unsigned int sizeOfBuffer);
    void Release();

#if defined(XCGRAPHICS_DX12)
    //Constant buffer heap
    ID3D12Resource*             m_cbResource;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
#else
    ID3D11Buffer*               m_cpuHandle;    //Unused?
    ID3D11Buffer*               m_gpuHandle;
    D3DShaderResourceView*      m_cbResource;   //SRV
#endif
    
    unsigned int*               m_cbDataBegin;
    BufferType                  m_bufferType;
    bool                        m_isInUse;
    unsigned int                m_sizeOfBuffer;
};