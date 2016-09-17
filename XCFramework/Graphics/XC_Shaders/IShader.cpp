/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/XC_Shaders/IShader.h"
#include "Graphics/GPUResourceSystem.h"
#include "Graphics/GPUResource.h"

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

GPUResource* IShader::CreateBuffer(GPUResourceType bufferType, i32 sizeOfType)
{
    GPUResource* constantBuff = nullptr;
    if (bufferType == GPUResourceType_CBV)
    {
        GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
        constantBuff = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(bufferType, sizeOfType));
    }
    else
    {
        XCASSERT(false); //Won't happen ever. Do it in Texture2D.cpp
    }

    return constantBuff;
}

void IShader::Destroy()
{
#if defined(USE_D3D_COMPILER)
    ReleaseCOM(m_pPS);
    ReleaseCOM(m_pVS);
#else
    if (m_pVS)
        XCDELETE(m_pVS);
    if (m_pPS)
        XCDELETE(m_pPS);
#endif
}