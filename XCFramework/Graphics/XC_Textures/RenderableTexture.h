/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/D3DConstantBuffer.h"
#include "Graphics/RenderTargetTypes.h"

class RenderableTexture
{
public:

    struct RenderedTextureInfo
    {
        ~RenderedTextureInfo()
        {
            XCDELETE(m_texData);
        }

        u8*     m_texData;
        u32     m_texSize;
    };

    RenderableTexture(RenderTargetsType type, ID3DDevice& device, ID3DDeviceContext& context);
    virtual ~RenderableTexture();

    bool                             PreLoad(i32 msaaQuality, i32 texWidth, i32 texHeight);

    //Creates a Render target from the swap chain's back buffer.
    void                             PreLoad(ID3DSwapChain* backbuffer);
    void                             Update();
    void                             Destroy();
    void                             OnResize();

    void                             SetRenderableTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView);
    void                             ClearRenderTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView, const XCVec4& color);
                                     
    D3DConstantBuffer*               GetShaderResourceView() { return m_pSRV; }
    ID3DTexture2D*                   GetTexture2D() { return m_pRenderTargetTexture;  }
                                     
    RenderedTextureInfo*             GetRenderToTexture();

protected:
#if defined(XCGRAPHICS_DX11)
    void                             CreateStagingTextures(D3D_TEXTURE2D_DESC texDesc);
#endif

    void                             DumpTextureToFile();

private:
    ID3DTexture2D*                   m_pRenderTargetTexture;
    ID3DRenderTargetView*            m_pRenderTargetView;
    D3DConstantBuffer*               m_pSRV;

    //Staging textures and for resolving the textures
    ID3DTexture2D*                   m_pRenderTargetTextureStaged;
    ID3DTexture2D*                   m_pSingleSampledTex;
                                     
    RenderedTextureInfo*             m_renderableTexture;
                                     
    ID3DDevice&                      m_device;
    ID3DDeviceContext&               m_deviceContext;
    RenderTargetsType                m_renderTargetType;
};