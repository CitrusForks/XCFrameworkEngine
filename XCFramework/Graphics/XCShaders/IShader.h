/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#endif

#include "Graphics/GPUResourceType.h"
#include "Graphics/XCPipelineStateObject/PSODx12.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"

class GPUResource;

class IShader
{
public:
    static const u32 NbOfDescriptors = 3;

    IShader()
    {}

    virtual ~IShader() 
    {
        Logger("ISHADER] Destructed shader");
    }

    virtual void                     Load(const void* fbBuffer);
    virtual void                     LoadShader() = 0;
    virtual void                     CreateConstantBuffers();
    virtual void                     ApplyShader(ID3DDeviceContext& context, RasterType rasterType = RasterType_FillSolid);
    virtual void                     Reset(ID3DDeviceContext& context) = 0;
    virtual void                     Destroy();

    UINT8*                           GetVertexShader() { return m_pVS; }
    UINT8*                           GetPixelShader()  { return m_pPS; }

#if defined(XCGRAPHICS_DX12)          
    PSODx12&                        GetPso() { return *m_pso; }
#endif

    GPUResource*                     CreateBuffer(GPUResourceType bufferType, i32 sizeOfType = 0);
                                     
protected:                           
    UINT8*                           m_pVS;
    UINT8*                           m_pPS;
    u32                              m_vsSize;
    u32                              m_psSize;

#if defined(XCGRAPHICS_DX12)
    PSODx12*                         m_pso;
#elif defined(XCGRAPHICS_DX11)
    ID3DVertexShader*                m_compiledVS;
    ID3DPixelShader*                 m_compiledPS;
#endif

    std::string                      m_vertexShaderPath;
    std::string                      m_pixelShaderPath;
};