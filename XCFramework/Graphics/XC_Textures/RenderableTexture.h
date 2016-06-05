/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_GNM)

class RenderableTexture
{
public:

    struct RenderedTextureInfo
    {
        unsigned char*  m_texData;
        unsigned int    m_texSize;
    };

    RenderableTexture(ID3DDevice& device, ID3DDeviceContext& context);
    virtual ~RenderableTexture();

    bool                        PreLoad(int msaaQuality, int texWidth, int texHeight);
    void                        PreLoad(ID3DTexture2D* backbuffer);
    void                        Update();
    void                        Destroy();
    void                        OnResize();

    void                        SetRenderableTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView);
    void                        ClearRenderTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView, const XCVec4& color);

    ID3DShaderResourceView*     GetShaderResourceView() { return m_pSRV; }
    ID3DTexture2D*              GetTexture2D() { return m_pRenderTargetTexture;  }

    void                        DumpTextureToFile();
    RenderedTextureInfo*        GetRenderToTexture();

#if defined(XCGRAPHICS_GNM)
    sce::Gnm::RenderTarget&          GetRenderTarget()        { return *m_pRenderTargetView; }
    sce::Gnm::DepthRenderTarget&     GetDepthRenderTarget()   { return m_gnmDepthTarget; }
#endif

private:
    ID3DTexture2D*              m_pRenderTargetTexture;
    ID3DRenderTargetView*       m_pRenderTargetView;
    ID3DShaderResourceView*     m_pSRV;

    ID3DTexture2D*              m_pRenderTargetTextureStaged;
    ID3DTexture2D*              m_pSingleSampledTex;

    RenderedTextureInfo*        m_renderableTexture;

    ID3DDevice&                 m_device;
    ID3DDeviceContext&          m_deviceContext;

#if defined(XCGRAPHICS_GNM)
    sce::Gnm::DepthRenderTarget m_gnmDepthTarget;
#endif
};

#endif