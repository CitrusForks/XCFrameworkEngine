/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

enum BufferType
{
    BUFFERTYPE_SRV,
    BUFFERTYPE_CBV,
    BUFFERTYPE_UAV
};

class D3DBufferDesc
{
public:
    D3DBufferDesc(BufferType type, u32 size)
        : m_bufferType(type), m_bufferSize(size)
    {
    }

    BufferType   m_bufferType;
    u32 m_bufferSize;
};

class D3DConstantBuffer
{
public:

    explicit D3DConstantBuffer(BufferType type);
    ~D3DConstantBuffer();

    void Release();

    void UploadZeroMemoryDataOnGPU(ID3DDeviceContext& context, u32 sizeOfBuffer);
    void UploadDataOnGPU(ID3DDeviceContext& context, void* buffer, u32 sizeOfBuffer);

    //Constant buffer heap
    GPU_DESCRIPTOR_HANDLE        m_gpuHandle;
    CPU_DESCRIPTOR_HANDLE        m_cpuHandle;
    ID3DShaderResourceView*      m_cbResource;
    
    u32*                         m_cbDataBegin;
    BufferType                   m_bufferType;
    bool                         m_isInUse;
    u32                          m_sizeOfBuffer;
};