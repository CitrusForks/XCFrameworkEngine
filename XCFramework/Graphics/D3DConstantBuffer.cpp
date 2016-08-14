/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "D3DConstantBuffer.h"

D3DConstantBuffer::D3DConstantBuffer(BufferType type)
    : m_cbResource(nullptr)
    , m_cbDataBegin(nullptr)
    , m_bufferType(type)
    , m_isInUse(false)
    , m_sizeOfBuffer(0)
{
#if defined(XCGRAPHICS_DX11)
    m_gpuHandle = nullptr;
    m_cpuHandle = nullptr;
#endif
}

D3DConstantBuffer::~D3DConstantBuffer()
{
    Release();
}

void D3DConstantBuffer::UploadZeroMemoryDataOnGPU(ID3DDeviceContext& context, u32 sizeOfBuffer)
{
    XCASSERT(m_sizeOfBuffer >= sizeOfBuffer);

#if defined(XCGRAPHICS_DX12)
    memset(m_cbDataBegin, 0, sizeOfBuffer);
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

void D3DConstantBuffer::UploadDataOnGPU(ID3DDeviceContext& context, void* buffer, u32 sizeOfBuffer)
{
    XCASSERT(m_sizeOfBuffer >= sizeOfBuffer);

#if defined(XCGRAPHICS_DX12)
    memcpy(m_cbDataBegin, buffer, sizeOfBuffer);
#elif defined(XCGRAPHICS_DX11)
    //Map, copy and unmap
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(context.Map(m_gpuHandle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    memcpy(mappedData.pData, buffer, sizeOfBuffer);
    context.Unmap(m_gpuHandle, 0);
#endif
}

void D3DConstantBuffer::Release()
{
    if (m_cbDataBegin)
    {
        //Dx12 : Not sure of this. Because the m_cbResource gpu memory is mapped on cpu and is controlled by dx. Anyway if at all the memory is not deallocated, it will be.
        delete m_cbDataBegin;
    }

    ReleaseCOM(m_cbResource);

#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_cpuHandle);
    ReleaseCOM(m_gpuHandle);
#endif
}