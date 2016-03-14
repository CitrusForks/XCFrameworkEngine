/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX12)
#include "Libs/Dx12Helpers/d3dx12.h"
#include "Engine/Graphics/XC_PipelineStateObject/PSO_Dx12.h"
#include "Engine/Graphics/D3DConstantBuffer.h"
#endif

#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"


class IShader
{
public:
    static const unsigned int NbOfDescriptors = 3;

    IShader(ID3DDevice& device)
        : m_device(device)
    {}

    virtual ~IShader() 
    {
        Logger("ISHADER] Destructed shader");
    }

    virtual void load(const void* fbBuffer);
    virtual void loadShader() = 0;
    virtual void createBufferConstants();
    virtual void applyShader(ID3DDeviceContext& context);
    virtual void destroy();
    virtual void reset(ID3DDeviceContext& context) = 0;
                                     
    ID3DVertexShader*                getVertexShader() { return m_pVS; }
    ID3DPixelShader*                 getPixelShader() { return m_pPS; }

#if defined(XCGRAPHICS_DX12)          
    PSO_Dx12&                        getPso() { return *m_pso; }
    D3DConstantBuffer*               createBuffer(BufferType bufferType, int sizeOfType = 0);
#elif defined(XCGRAPHICS_DX11)
    ID3D11InputLayout*               getInputLayout() { return m_pInputLayout; }
    ID3D11Buffer*                    createBuffer(int sizeOfType);
#elif defined(XCGRAPHICS_GNM)
    template<class T>
    void                             createBuffer(ID3DDeviceContext& context, T** inBuffer, sce::Gnm::Buffer& outBuffer)
    {
        T* buffer = static_cast<T*>(context.allocateFromCommandBuffer(sizeof(T), sce::Gnm::kEmbeddedDataAlignment4));
        (*inBuffer) = buffer;
        outBuffer.initAsConstantBuffer((*inBuffer), sizeof(T));
    }
#elif defined(XCGRAPHICS_GL)
    D3DConstantBuffer*               createBuffer(int sizeOfType) { return nullptr; }
#endif                               
                                     
protected:                           
    void                             MemCpyConstants(ID3DDeviceContext& context, void* dest, void* src, unsigned int size);

    ID3DDevice&                      m_device;

    ID3DVertexShader*                m_pVS;
    ID3DPixelShader*                 m_pPS;
    unsigned int                     m_vsSize;
    unsigned int                     m_psSize;

#if defined(XCGRAPHICS_DX12)
    PSO_Dx12*                        m_pso;
#elif defined(XCGRAPHICS_DX11)
    ID3D11InputLayout*               m_pInputLayout;
#elif defined(XCGRAPHICS_GNM)
    void*                            m_fetchVSShader;
    uint32_t                         m_shaderModifier;
#endif

    std::string                      m_vertexShaderPath;
    std::string                      m_pixelShaderPath;
};