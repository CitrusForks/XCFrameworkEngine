/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#ifndef __CUBEMAPSHADER__
#define __CUBEMAPSHADER__

#include "Engine/Utils/GraphicUtils.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/src/IShader.h"

class CubeMapShader : public IShader
{
public:
    static const unsigned int NbOfDescriptors = 1;

    CubeMapShader(ID3DDevice& device)
        : IShader(device)
    {}
    virtual                     ~CubeMapShader() {}

    void                        loadShader();
    void                        setCubeTexture2D(ID3DDeviceContext& context, D3DShaderResourceView* tex);
    void                        applyShader(ID3DDeviceContext& context);
    void                        createBufferConstants();

    void                        setWVP(ID3DDeviceContext& context, cbWVP& wvp);

#if defined(XCGRAPHICS_DX12)
    void                        setWVP(ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
#endif

    void                        destroy();
    void                        reset(ID3DDeviceContext& context);

private:
    D3DShaderResourceView*      m_pTexture2DCubeMap;  //Mapped with gDiffuseMap
#if defined(XCGRAPHICS_DX12) || defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_GL)
    D3DConstantBuffer*          m_wvp;
#elif defined(XCGRAPHICS_GNM)
    sce::Gnm::Buffer            m_cbWvp;
    cbWVP*                      m_wvp;
#endif
};

#endif