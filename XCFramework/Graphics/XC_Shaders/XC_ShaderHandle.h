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
        D3D12_SHADER_BUFFER_DESC     constantBufferDesc;
        //Texture slots and others
        D3D12_SHADER_INPUT_BIND_DESC resourceBufferDesc;
    };

    BufferDesc   m_bufferDesc;
    BufferType   m_bufferType;

    std::string GetBufferName()
    {
        return m_bufferType == BUFFERTYPE_CBV ? m_bufferDesc.constantBufferDesc.Name : m_bufferDesc.resourceBufferDesc.Name;
    }

    ShaderSlot(D3D12_SHADER_BUFFER_DESC buffer)
    {
        m_bufferDesc.constantBufferDesc = buffer;
        m_bufferType = BUFFERTYPE_CBV;
    }

    ShaderSlot(D3D12_SHADER_INPUT_BIND_DESC buffer)
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
    void            SetConstantBuffer(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
    void            SetSampler(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
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
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
        switch (m_vertexFormat)
        {
        case VertexFormat_Position:
            vertexBufferView = ((VertexBuffer<VertexPosColor>*)vertexBuffer)->GetVertexBufferView();
            break;
        case VertexFormat_PositionNormalTexture:
            vertexBufferView = ((VertexBuffer<VertexPosNormTex>*)vertexBuffer)->GetVertexBufferView();
            break;

        case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
            vertexBufferView = ((VertexBuffer<VertexPosNormTexBIndexBWeight>*)vertexBuffer)->GetVertexBufferView();
            break;

        case VertexFormat_PositionColorInstanceIndex:
            vertexBufferView = ((VertexBuffer<VertexPosColorInstanceIndex>*)vertexBuffer)->GetVertexBufferView();
            break;

        default:
            XCASSERT(false);
        }

#if defined(XCGRAPHICS_DX12)
        context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.IASetVertexBuffers(0, 1, &vertexBufferView);
#elif defined(XCGRAPHICS_DX11)
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        unsigned int stride = sizeof(T);
        unsigned int offset = 0;
        context.IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
#endif
    }

    void SetIndexBuffer(ID3DDeviceContext& context, IndexBuffer<unsigned int>& indexBuffer)
    {
#if defined(XCGRAPHICS_DX12)
        context.IASetIndexBuffer(&indexBuffer.GetIndexBufferView());
#elif defined(XCGRAPHICS_DX11)
        UINT32 offset = 0;
        context.IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, offset);
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
    std::vector<D3D12_SIGNATURE_PARAMETER_DESC> m_shaderInputParams;

    ID3D12ShaderReflection*                     m_vsShaderReflection;
    ID3D12ShaderReflection*                     m_psShaderReflection;
    D3D12_INPUT_LAYOUT_DESC                     m_inputLayoutDesc;

    bool                                        m_enableDepth;
    VertexFormat                                m_vertexFormat;
};