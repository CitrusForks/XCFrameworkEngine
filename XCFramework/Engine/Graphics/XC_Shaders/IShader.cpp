/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "IShader.h"
#include "Engine/Graphics/SharedDescriptorHeap.h"
#include "Assets/Packages/PackageConsts.h"

void IShader::Load(const void* fbBuffer)
{
    const FBShader* fbShader = (FBShader*)fbBuffer;
    m_vertexShaderPath = fbShader->VsPath()->c_str();
    m_pixelShaderPath  = fbShader->PsPath()->c_str();
}

void IShader::CreateConstantBuffers()
{
}

void IShader::ApplyShader(ID3DDeviceContext& context, RasterType rasterType)
{
}

void IShader::MemCpyConstants(ID3DDeviceContext& context, void* dest, void* src, unsigned int size)
{
#if defined(XCGRAPHICS_DX11)
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context.Map((ID3D11Buffer*)dest, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    std::memcpy(mappedResource.pData, src, size);
    context.Unmap((ID3D11Buffer*)dest, 0);
#endif
}

#if defined(XCGRAPHICS_DX12)
D3DConstantBuffer* IShader::CreateBuffer(BufferType bufferType, int sizeOfType)
{
    D3DConstantBuffer* constantBuff = nullptr;
    if (bufferType == BUFFERTYPE_CBV)
    {
        SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");

        D3DBufferDesc bufferDesc(bufferType, sizeOfType);
        constantBuff = heap.CreateBufferView(bufferDesc);
    }
    else
    {
        XCASSERT(false); //Won't happen ever. Do it in Texture2D.cpp
    }

    return constantBuff;
}

#elif defined(XCGRAPHICS_DX11)
ID3D11Buffer* IShader::CreateBuffer(int sizeOfType)
{
    ID3D11Buffer* buffer;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeOfType;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ValidateResult(m_device.CreateBuffer(&bd, nullptr, &buffer));

    return buffer;
}


#endif

void IShader::destroy()
{
#if defined(XCGRAPHICS_DX12)


#if defined(USE_D3D_COMPILER)
    ReleaseCOM(m_pPS);
    ReleaseCOM(m_pVS);
#else
    if (m_pVS)
        delete m_pVS;
    if (m_pPS)
        delete m_pPS;
#endif
#elif defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_pInputLayout);

    ReleaseCOM(m_pPS);
    ReleaseCOM(m_pVS);
#endif


}