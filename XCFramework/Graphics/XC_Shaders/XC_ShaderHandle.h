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

    //Texture slots
    "gDiffuseMap",
    "gDiffuseMap1",
    "gDiffuseMap2",
    "gBlendMap",
    "gCubeMap",

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

    BufferDesc   m_bufferDesc;
    BufferType   m_bufferType;

    std::string GetBufferName()
    {
        return m_bufferType == BUFFERTYPE_CBV ? m_bufferDesc.constantBufferDesc.Name : m_bufferDesc.resourceBufferDesc.Name;
    }

    ShaderSlot(D3D_SHADER_BUFFER_DESC buffer)
    {
        m_bufferDesc.constantBufferDesc = buffer;
        m_bufferType = BUFFERTYPE_CBV;
    }

    ShaderSlot(D3D_SHADER_INPUT_BIND_DESC buffer)
    {
        m_bufferDesc.resourceBufferDesc = buffer;
        m_bufferType = BUFFERTYPE_SRV;
    }
};

class XCShaderHandle : public IShader
{
public:

    XCShaderHandle(ID3DDevice& device);
    ~XCShaderHandle();

    void            Load(const void* fbBuffer);
    void            ApplyShader(ID3DDeviceContext& context, RasterType rasterType = RasterType_FillSolid);
    void            Reset(ID3DDeviceContext& context) {}
    void            SetConstantBuffer(std::string bufferName, ID3DDeviceContext& context, D3DConstantBuffer& constantBuffer);
    
#if defined(XCGRAPHICS_DX12)
    void            SetSampler(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
#elif defined(XCGRAPHICS_DX11)
    void            SetSampler(std::string bufferName, ID3DDeviceContext& context, ID3D11SamplerState* samplerState);
#endif

    void            SetResource(std::string bufferName, ID3DDeviceContext& context, ResourceHandle* tex);
    VertexFormat    GetVertexFormat() { return m_vertexFormat; }

    void* CreateVertexBuffer()
    {
        void* buffer = nullptr;
        switch (m_vertexFormat)
        {
        case VertexFormat_Position:
            buffer = new VertexBuffer<VertexPos>();
            break;

        case VertexFormat_PositionColor:
            buffer = new VertexBuffer<VertexPosColor>();
            break;

        case VertexFormat_PositionNormalTexture:
            buffer = new VertexBuffer<VertexPosNormTex>();
            break;

        case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
            buffer = new VertexBuffer<VertexPosNormTexBIndexBWeight>();
            break;

        case VertexFormat_PositionColorInstanceIndex:
            buffer = new VertexBuffer<VertexPosColorInstanceIndex>();
            break;

        default:
            XCASSERT(false);
        }

        return buffer;
    }

    D3DConstantBuffer*  CreateConstantBuffer(std::string constantBufferName)
    {
        SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
        return heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, GetSizeOfConstantBuffer(constantBufferName)));
    }

    void DestroyConstantBuffer(D3DConstantBuffer* constantBuffer)
    {
        SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
        heap.DestroyBuffer(constantBuffer);
    }

    void SetVertexBuffer(ID3DDeviceContext& context, void* vertexBuffer)
    {
        switch (m_vertexFormat)
        {
        case VertexFormat_Position:
            ((VertexBuffer<VertexPosColor>*)vertexBuffer)->SetVertexBuffer(context);
            break;
        case VertexFormat_PositionNormalTexture:
            ((VertexBuffer<VertexPosNormTex>*)vertexBuffer)->SetVertexBuffer(context);
            break;

        case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
            ((VertexBuffer<VertexPosNormTexBIndexBWeight>*)vertexBuffer)->SetVertexBuffer(context);
            break;

        case VertexFormat_PositionColorInstanceIndex:
            ((VertexBuffer<VertexPosColorInstanceIndex>*)vertexBuffer)->SetVertexBuffer(context);
            break;

        default:
            XCASSERT(false);
        }
    }

    void SetIndexBuffer(ID3DDeviceContext& context, IndexBuffer<unsigned int>& indexBuffer)
    {
#if defined(XCGRAPHICS_DX12)
        context.IASetIndexBuffer(&indexBuffer.GetIndexBufferView());
#elif defined(XCGRAPHICS_DX11)
        UINT32 offset = 0;
        context.IASetIndexBuffer(indexBuffer.m_pIB, DXGI_FORMAT_R32_UINT, offset);
#elif defined(XCGRAPHICS_GNM)
        //Specified while drawIndex() call. See below GetIndexBufferInGPUMem
#endif
    }

protected:

    void                    LoadShader();
    void                    SortSlots();
    unsigned int            GetSlotPriority(std::string bufferName);
    void                    GenerateRootSignature();
    void                    GeneratePSO();
    void                    ReadShaderDescription();
    unsigned int            GetSizeOfConstantBuffer(std::string& cbName);

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