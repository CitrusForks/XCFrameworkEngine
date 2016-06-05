/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum BufferType
{
    BUFFERTYPE_SRV,
    BUFFERTYPE_CBV,
    BUFFERTYPE_UAV
};

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#include "Graphics/Utils/GraphicUtils.h"

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
    D3DConstantBuffer()
    {}

    D3DConstantBuffer(BufferType type)
        : m_bufferType(type)
    {
        m_isInUse = false;
    }

    //Constant buffer heap
    ID3D12Resource*             m_cbResource;
    unsigned int*               m_cbDataBegin;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
    BufferType                  m_bufferType;
    bool                        m_isInUse;
    unsigned int                m_sizeOfBuffer;

    void                        Release()
    {
        ReleaseCOM(m_cbResource);
    }
};

typedef D3DConstantBuffer D3DShaderResourceView;

#endif