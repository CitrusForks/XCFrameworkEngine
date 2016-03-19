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

class XC_ShaderManager
{
public:
    XC_ShaderManager(ID3DDevice& device);
    ~XC_ShaderManager(void);

    void                                    Init();
    void                                    LoadShaders();
    void                                    Destroy();
    IShader*                                GetShader(SHADERTYPE);

    void                                    ApplyShader(ID3DDeviceContext& context, SHADERTYPE _shaderType);
    void                                    DrawNonIndexed(ID3DDeviceContext& context, unsigned int vertexCount);
    void                                    DrawIndexedInstanced(ID3DDeviceContext& context, unsigned int _indexCount, void* indexGpuAddr = nullptr, unsigned int instanceCount = 1);

    void                                    ClearShaderAndRenderStates(ID3DDeviceContext& context);

#if defined(XCGRAPHICS_DX11)
    void                                    LoadRasterizers();
    void                                    SetRasterizerState(ID3DDeviceContext& context, ERasterizer_Type type);
#endif

private:

    std::vector<IShader*>                                 m_Shaders;

#if defined(XCGRAPHICS_DX12)
    D3D12_RASTERIZER_DESC                                 m_rasterizerDesc;
    D3D12_SAMPLER_DESC                                    m_samplerDesc;
    SharedDescriptorHeap*                                 m_sharedDescriptorHeap;
#elif defined(XCGRAPHICS_DX11)
    D3D11_RASTERIZER_DESC                                 m_rasterizerDesc;
    std::map<ERasterizer_Type, ID3D11RasterizerState*>    m_rasterizerStates;

    D3D11_SAMPLER_DESC                                    m_samplerDesc;
    ID3D11SamplerState*                                   m_SamplerLinear;
#endif

    ID3DDevice&                                           m_device;
};