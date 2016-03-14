/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/IShader.h"

//Add all the slots priority wise
static std::vector<std::string> gs_slotPriority =
{
    //Constant buffers per object
    "cbPerObjectBuffer",
    "cbWVP",
    "cbSkinnedCharacterBuffer",

    //Constant buffers per frame
    "cbLightsPerFrame",
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

    std::string getBufferName()
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

    void            load(const void* fbBuffer);
    void            loadShader();
    void            applyShader(ID3DDeviceContext& context);
    void            reset(ID3DDeviceContext& context) {}
    void            setConstantBuffer(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
    void            setResource(std::string bufferName, ID3DDeviceContext& context, D3DShaderResourceView* tex);
    
protected:
    void            sortSlots();
    unsigned int    getSlotPriority(std::string bufferName);
    void            generateRootSignature();
    void            readShaderDescription();
    void            generatePSO();

private:
    std::vector<ShaderSlot>                     m_shaderSlots;
    std::vector<D3D12_SIGNATURE_PARAMETER_DESC> m_shaderInputParams;

    ID3D12ShaderReflection*                     m_vsShaderReflection;
    ID3D12ShaderReflection*                     m_psShaderReflection;
    D3D12_INPUT_LAYOUT_DESC                     m_inputLayoutDesc;

    bool                                        m_enableDepth;
};