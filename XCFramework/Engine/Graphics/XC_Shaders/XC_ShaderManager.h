/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"
#include "Engine/Graphics/XC_Shaders/IShader.h"
#include "Engine/Graphics/SharedDescriptorHeap.h"
#include "Engine/FlatBuffersInterface/FlatBuffersSystem.h"

class XC_ShaderManager
{
public:
    XC_ShaderManager(ID3DDevice& device);
    ~XC_ShaderManager(void);

    void                  Init();
    void                  LoadShaders();
    void                  Destroy();

    void                  SetRasterizerState(ID3DDeviceContext& context, RasterType type);
    IShader*              GetShader(ShaderType shaderType);
    ID3D12DescriptorHeap* GetSamplerDescriptorHeap() { return m_samplerHeap; }

    void                  ApplyShader(ID3DDeviceContext& context, ShaderType _ShaderType);

    void                  DrawNonIndexed(ID3DDeviceContext& context, unsigned int vertexCount);
    void                  DrawIndexedInstanced(ID3DDeviceContext& context, unsigned int _indexCount, void* indexGpuAddr = nullptr, unsigned int instanceCount = 1);
                          
    void                  ClearShaderAndRenderStates(ID3DDeviceContext& context);
                          
    void                  LoadRasterizers();
    void                  LoadSamplers();

private:

    std::vector<IShader*>       m_Shaders;

#if defined(XCGRAPHICS_DX12)
    D3D12_RASTERIZER_DESC       m_rasterizerDesc;
    
    //Samplers
    D3D12_SAMPLER_DESC          m_samplerDesc;
    ID3D12DescriptorHeap*       m_samplerHeap;

    SharedDescriptorHeap*       m_sharedDescriptorHeap;
#elif defined(XCGRAPHICS_DX11)
    D3D11_RASTERIZER_DESC                                 m_rasterizerDesc;
    std::map<RasterType, ID3D11RasterizerState*>    m_rasterizerStates;

    D3D11_SAMPLER_DESC                                    m_samplerDesc;
    ID3D11SamplerState*                                   m_SamplerLinear;
#endif

    ID3DDevice&                 m_device;
    RasterType                  m_rasterType;
    FlatBuffersSystem::FBBuffer m_fbBuffer;
};