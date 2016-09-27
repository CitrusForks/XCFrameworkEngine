/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Shaders/IShader.h"
#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/GPUResource.h"

#include "Engine/Resource/ResourceHandle.h"

//Add all the slots priority wise
static const std::vector<std::string> gs_slotPriority =
{
    //Constant buffers per object
    "cbLightsPerFrame",
    "PerObjectBuffer",
    "cbInstancedBuffer",
    "cbWVP",
    "cbBoneBuffer",

    //RTV treated as SRV
    "gGBufferDiffuse",
    "gGBufferPosition",
    "gGBufferNormal",

    //Texture slots
    "gDiffuseMap",
    "gDiffuseMap1",
    "gDiffuseMap2",
    "gBlendMap",
    "gCubeMap",
    "gTexture",

    //Samplers all the way in the last
    "samLinear"
};

struct ShaderSlot
{
    union BufferDesc
    {
        //Constant buffers
        D3D_SHADER_BUFFER_DESC     constantBufferDesc;
        //Texture slots and others
        D3D_SHADER_INPUT_BIND_DESC resourceBufferDesc;
    };

    std::string GetBufferName()
    {
        return m_bufferType == GPUResourceType_CBV ? m_bufferDesc.constantBufferDesc.Name : m_bufferDesc.resourceBufferDesc.Name;
    }

    ShaderSlot(D3D_SHADER_BUFFER_DESC buffer)
    {
        m_bufferDesc.constantBufferDesc = buffer;
        m_bufferType = GPUResourceType_CBV;
    }

    ShaderSlot(D3D_SHADER_INPUT_BIND_DESC buffer)
    {
        m_bufferDesc.resourceBufferDesc = buffer;
        m_bufferType = GPUResourceType_SRV;
    }

    BufferDesc      m_bufferDesc;
    GPUResourceType m_bufferType;
    u32             m_shaderSlotId;
};

class XCShaderHandle : public IShader
{
public:

    XCShaderHandle(ID3DDevice& device);
    ~XCShaderHandle();

    void            Load(const void* fbBuffer);
    void            ApplyShader(ID3DDeviceContext& context, RasterType rasterType = RasterType_FillSolid);
    void            Reset(ID3DDeviceContext& context) {}
    void            SetConstantBuffer(std::string bufferName, ID3DDeviceContext& context, GPUResource& constantBuffer);
    
#if defined(XCGRAPHICS_DX12)
    void            SetSampler(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
#elif defined(XCGRAPHICS_DX11)
    void            SetSampler(std::string bufferName, ID3DDeviceContext& context, ID3D11SamplerState* samplerState);
#endif

    void            SetResource(std::string bufferName, ID3DDeviceContext& context, GPUResource* tex);
    void            SetResource(std::string bufferName, ID3DDeviceContext& context, ResourceHandle* tex);
    VertexFormat    GetVertexFormat() { return m_vertexFormat; }

    void*           CreateVertexBuffer();
    GPUResource*    CreateConstantBuffer(std::string constantBufferName);
    void            DestroyConstantBuffer(GPUResource* constantBuffer);

    void            SetVertexBuffer(ID3DDeviceContext& context, void* vertexBuffer);
    void            SetIndexBuffer(ID3DDeviceContext& context, IndexBuffer<u32>& indexBuffer);

protected:

    void            LoadShader();
    void            SortSlots();
    u32             GetSlotPriority(std::string bufferName);
    void            GenerateRootSignature();
    void            GeneratePSO();
    void            ReadShaderDescription();
    u32             GetSizeOfConstantBuffer(std::string& cbName);

private:

    std::vector<ShaderSlot>                     m_shaderSlots;
    std::vector<D3D_SIGNATURE_PARAMETER_DESC>   m_shaderInputParams;

    ID3DShaderReflection*                       m_vsShaderReflection;
    ID3DShaderReflection*                       m_psShaderReflection;
    D3D_INPUT_LAYOUT_DESC                       m_inputLayoutDesc;

#if defined(XCGRAPHICS_DX11)
    ID3D11InputLayout*                          m_inputLayout;
#endif

    bool                                        m_enableDepth;
    VertexFormat                                m_vertexFormat;
};