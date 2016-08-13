/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Graphics/XC_Shaders/XC_RasterizerTypes.h"
#include "Graphics/XC_Shaders/GlobalShaderData.h"

#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"

class IShader;
class SharedDescriptorHeap;

class XC_ShaderContainer
{
public:
    XC_ShaderContainer(ID3DDevice& device);
    ~XC_ShaderContainer(void);

    void                        Init();
    void                        LoadShaders();
    void                        Destroy();

    void                        ApplyShader(ID3DDeviceContext& context, ShaderType _ShaderType);
    void                        ClearShaderAndRenderStates(ID3DDeviceContext& context);

    void                        LoadRasterizers();
    void                        LoadSamplers();

    IShader*                    GetShader(ShaderType shaderType);
    GlobalShaderData&           GetGlobalShaderData() { return m_globalShaderData; }
#if defined(XCGRAPHICS_DX12)
    ID3D12DescriptorHeap*       GetSamplerDescriptorHeap() { return m_samplerHeap; }
#endif

    void                        SetRasterizerState(ID3DDeviceContext& context, RasterType type);

private:

    std::vector<IShader*>       m_Shaders;

#if defined(XCGRAPHICS_DX12)
    D3D12_RASTERIZER_DESC       m_rasterizerDesc;
    
    //Samplers
    D3D12_SAMPLER_DESC          m_samplerDesc;
    ID3D12DescriptorHeap*       m_samplerHeap;

#elif defined(XCGRAPHICS_DX11)
    D3D11_RASTERIZER_DESC                        m_rasterizerDesc;
    std::map<RasterType, ID3D11RasterizerState*> m_rasterizerStates;

    D3D11_SAMPLER_DESC                           m_samplerDesc;
    ID3D11SamplerState*                          m_SamplerLinear;
#endif

    SharedDescriptorHeap*       m_sharedDescriptorHeap;
    ID3DDevice&                 m_device;
    RasterType                  m_rasterType;
    FlatBuffersSystem::FBBuffer m_fbBuffer;
    GlobalShaderData            m_globalShaderData;
};