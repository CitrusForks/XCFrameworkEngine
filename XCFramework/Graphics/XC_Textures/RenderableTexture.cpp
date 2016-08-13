/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "RenderableTexture.h"
#include "Engine/Input/Directinput.h"
#include "Graphics/SharedDescriptorHeap.h"

#if defined(XCGRAPHICS_DX11) 
#include "Libs/DirectXTex/DirectXTex/DirectXTex.h"
#endif

RenderableTexture::RenderableTexture(ID3DDevice& device, ID3DDeviceContext& context)
    : m_pRenderTargetTexture(nullptr)
    , m_pRenderTargetView(nullptr)
    , m_pSRV(nullptr)
    , m_pRenderTargetTextureStaged(nullptr)
    , m_pSingleSampledTex(nullptr)
    , m_device(device)
    , m_deviceContext(context)
{
    m_renderableTexture = XCNEW(RenderedTextureInfo)();

#if defined(XCGRAPHICS_GNM)
    m_pRenderTargetView = new ID3DRenderTargetView;
#endif
    
}

RenderableTexture::~RenderableTexture()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_pRenderTargetView);
    ReleaseCOM(m_pRenderTargetTexture);
    ReleaseCOM(m_pSRV);
#elif defined(XCGRAPHICS_GNM)
    XCDELETE(m_pRenderTargetView);
#endif
}

void RenderableTexture::Destroy()
{
    XCDELETE(m_renderableTexture->m_texData);
    XCDELETE(m_renderableTexture);

    if (m_pSRV)
    {
        XCDELETE(m_pSRV);
    }
}

void RenderableTexture::OnResize()
{
#if defined(XCGRAPHICS_DX11)
    m_pRenderTargetView->Release();
#endif
}

bool RenderableTexture::PreLoad(i32 msaaQuality, i32 texWidth, i32 texHeight)
{
#if defined(XCGRAPHICS_DX11)
    D3D_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = texWidth;
    textureDesc.Height = texHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 4;
    textureDesc.SampleDesc.Quality = msaaQuality - 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    ValidateResult(m_device.CreateTexture2D(&textureDesc, NULL, &m_pRenderTargetTexture));

    D3D_TEXTURE2D_DESC stagedTexDesc = { 0 };
    stagedTexDesc = textureDesc;
    stagedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    stagedTexDesc.SampleDesc.Count = 1;
    stagedTexDesc.SampleDesc.Quality = 0;
    stagedTexDesc.CPUAccessFlags = 0;
    stagedTexDesc.Usage = D3D11_USAGE_DEFAULT;
    ValidateResult(m_device.CreateTexture2D(&stagedTexDesc, NULL, &m_pRenderTargetTextureStaged));

    stagedTexDesc.BindFlags = 0;
    stagedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagedTexDesc.Usage = D3D11_USAGE_STAGING;
    ID3DTexture2D* singleSampledTex = nullptr;
    ValidateResult(m_device.CreateTexture2D(&stagedTexDesc, NULL, &m_pSingleSampledTex));

    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Create the render target view.
    ValidateResult(m_device.CreateRenderTargetView(m_pRenderTargetTexture, &renderTargetViewDesc, &m_pRenderTargetView));

    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    m_pSRV = XCNEW(D3DConstantBuffer)(BUFFERTYPE_SRV);

    ValidateResult(m_device.CreateShaderResourceView(m_pRenderTargetTextureStaged, &shaderResourceViewDesc, &m_pSRV->m_cbResource));

    // RGB Texture for transferring over the network
    m_renderableTexture->m_texData = XCNEW(u8)[texWidth * texHeight * 3];
    m_renderableTexture->m_texSize = texWidth * texHeight * 3;
#endif

    return true;
}

void RenderableTexture::PreLoad(ID3DTexture2D* backbuffer)
{
#if defined(XCGRAPHICS_DX11)
    m_pRenderTargetTexture = backbuffer;
    (m_device.CreateRenderTargetView(backbuffer, 0, &m_pRenderTargetView));
#endif
}

void RenderableTexture::Update()
{
#if defined(XCGRAPHICS_DX11)
    DirectInput& input = (DirectInput&)SystemLocator::GetInstance()->RequestSystem("InputSystem");
    if (input.KeyDown(DIK_R))
    {
        DumpTextureToFile();
    }

    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged, 0, m_pRenderTargetTexture, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
#endif
}

void RenderableTexture::SetRenderableTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView)
{
#if defined(XCGRAPHICS_DX11)
    context.OMSetRenderTargets(1, &m_pRenderTargetView, depthView);
#elif defined(XCGRAPHICS_GNM)
    context.setRenderTarget(0, m_pRenderTargetView);
    context.setDepthRenderTarget(&m_gnmDepthTarget);
#endif
}

void RenderableTexture::ClearRenderTarget(ID3DDeviceContext& context, ID3DDepthStencilView* depthView, const XCVec4& xmColor)
{
#if defined(XCGRAPHICS_DX11)
    const f32 color[] = { xmColor.Get<X>(), xmColor.Get<Y>(), xmColor.Get<Z>(), xmColor.Get<W>() };

    context.ClearRenderTargetView(m_pRenderTargetView, color);
    context.ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#endif
}

void RenderableTexture::DumpTextureToFile()
{
#if defined(XCGRAPHICS_DX11)
    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged, 0, m_pRenderTargetTexture, 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    D3D11_MAPPED_SUBRESOURCE subResource = { 0 };

    D3D_TEXTURE2D_DESC texDesc = { 0 };
    m_pRenderTargetTexture->GetDesc(&texDesc);

    m_deviceContext.CopyResource(m_pSingleSampledTex, m_pRenderTargetTextureStaged);

    ValidateResult(m_deviceContext.Map(m_pSingleSampledTex, 0, D3D11_MAP_READ, 0, &subResource));

    u8* pData = (u8*)subResource.pData;

    DirectX::Image image;
    image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    image.height = texDesc.Height;
    image.width = texDesc.Width;
    image.pixels = pData;
    image.rowPitch = subResource.RowPitch;
    image.slicePitch = subResource.DepthPitch;

    DirectX::SaveToDDSFile(image, 0, L"Output.dds");

    FILE* fp = fopen("Out.bmp", "wb");
    fwrite(pData, sizeof(u8), texDesc.Height * texDesc.Width * 4, fp);
    fclose(fp);

    m_deviceContext.Unmap(m_pSingleSampledTex, 0);
#endif
}

RenderableTexture::RenderedTextureInfo* RenderableTexture::GetRenderToTexture()
{
#if defined(XCGRAPHICS_DX11)
    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged, 0, m_pRenderTargetTexture, 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    D3D11_MAPPED_SUBRESOURCE subResource = { 0 };

    D3D_TEXTURE2D_DESC texDesc = { 0 };
    m_pRenderTargetTexture->GetDesc(&texDesc);

    m_deviceContext.CopyResource(m_pSingleSampledTex, m_pRenderTargetTextureStaged);

    ValidateResult(m_deviceContext.Map(m_pSingleSampledTex, 0, D3D11_MAP_READ, 0, &subResource));

    u8* pData = (u8*)subResource.pData;

    memcpy(m_renderableTexture->m_texData, subResource.pData, texDesc.Height * texDesc.Width * 4);
    
    /*u32 rgbImageIndex = 0;
    for (u32 pixelIndex = 0; pixelIndex < texDesc.Height * texDesc.Width * 4; pixelIndex += 4)
    {
        m_renderableTexture->m_texData[rgbImageIndex++] = pData[pixelIndex];
        m_renderableTexture->m_texData[rgbImageIndex++] = pData[pixelIndex + 1];
        m_renderableTexture->m_texData[rgbImageIndex++] = pData[pixelIndex + 2];
    }
    */

    m_deviceContext.Unmap(m_pSingleSampledTex, 0);
#endif

    return m_renderableTexture;
}