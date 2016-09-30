/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "RenderableTexture.h"

#include "Engine/Input/XCInput.h"

#include "Graphics/SharedDescriptorHeap.h"
#include "Graphics/GPUResourceSystem.h"

#if defined(XCGRAPHICS_DX11) 
#include "Libs/DirectXTex/DirectXTex.h"
#endif

RenderableTexture::RenderableTexture(RenderTargetsType type, DXGI_FORMAT format, ID3DDevice& device, ID3DDeviceContext& context)
    : m_pRenderTargetResource(nullptr)
    , m_pRenderTargetTextureStaged(nullptr)
    , m_pSingleSampledTex(nullptr)
    , m_device(device)
    , m_deviceContext(context)
    , m_renderTargetType(type)
    , m_format(format)
{
    m_renderableTexture = XCNEW(RenderedTextureInfo)();

#if defined(XCGRAPHICS_GNM)
    m_pRenderTargetView = new ID3DRenderTargetView;
#endif
}

RenderableTexture::~RenderableTexture()
{
#if defined(XCGRAPHICS_GNM)
    XCDELETE(m_pRenderTargetView);
#endif
}

void RenderableTexture::Destroy()
{
    if (m_renderableTexture)
    {
        XCDELETE(m_renderableTexture);
    }

    if (m_pRenderTargetResource)
    {
        GPUResourceSystem& gpuSys = (GPUResourceSystem&) SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
        gpuSys.DestroyResource(m_pRenderTargetResource);
        m_pRenderTargetResource = nullptr;
    }
}

void RenderableTexture::OnResize()
{
#if defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_DX12)
    m_pRenderTargetResource->GetResource<ID3DResource*>()->Release();
#endif
}

bool RenderableTexture::PreLoad(i32 msaaQuality, i32 texWidth, i32 texHeight)
{
    GPUResourceSystem& descHeap = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");

    // Initialize the render target texture description.
    D3D_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

#if defined(XCGRAPHICS_DX12)

    // Setup the render target texture description.
    textureDesc = CD3DX12_RESOURCE_DESC
    (
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        texWidth,
        texHeight,
        1,
        1,
        m_format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    m_pRenderTargetResource = descHeap.CreateTextureResource(textureDesc, nullptr);

    //Create srv view.
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    ZeroMemory(&desc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));

    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = m_format;
    desc.Texture2D.MipLevels = 1;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.ResourceMinLODClamp = 0.0f;

    //Create SRV of the render texture
    descHeap.CreateShaderResourceView(desc, m_pRenderTargetResource);

    // Create the render target texture.
    descHeap.CreateRenderTargetView(m_pRenderTargetResource);

#elif defined(XCGRAPHICS_DX11)

    // Setup the render target texture description.
    textureDesc.Width = texWidth;
    textureDesc.Height = texHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = m_format;
    if (msaaQuality)
    {
        textureDesc.SampleDesc.Count = 4;
        textureDesc.SampleDesc.Quality = msaaQuality - 1;
    }
    else
    {
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
    }

    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    m_pRenderTargetResource = descHeap.CreateTextureResource(textureDesc, nullptr);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    desc.ViewDimension = msaaQuality ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Format = m_format;
    desc.Texture2D.MipLevels = 1;
    desc.Texture2D.MostDetailedMip = 0;

    //Create SRV of the render texture
    descHeap.CreateShaderResourceView(desc, m_pRenderTargetResource);

    // Create the render target view.
    descHeap.CreateRenderTargetView(m_pRenderTargetResource);

    CreateStagingTextures(textureDesc);
#endif

    return true;
}

#pragma region StagingTexture
#if defined(XCGRAPHICS_DX11)
void RenderableTexture::CreateStagingTextures(D3D_TEXTURE2D_DESC texDesc)
{
    GPUResourceSystem& descHeap = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");

    D3D_TEXTURE2D_DESC stagedTexDesc = { 0 };
    stagedTexDesc = texDesc;
    stagedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    stagedTexDesc.SampleDesc.Count = 1;
    stagedTexDesc.SampleDesc.Quality = 0;
    stagedTexDesc.CPUAccessFlags = 0;
    stagedTexDesc.Usage = D3D11_USAGE_DEFAULT;

    m_pRenderTargetTextureStaged = descHeap.CreateTextureResource(stagedTexDesc, nullptr);

    stagedTexDesc.BindFlags = 0;
    stagedTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagedTexDesc.Usage = D3D11_USAGE_STAGING;
    
    m_pSingleSampledTex = descHeap.CreateTextureResource(stagedTexDesc, nullptr);

    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    shaderResourceViewDesc.Format = texDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    descHeap.CreateShaderResourceView(shaderResourceViewDesc, m_pRenderTargetTextureStaged);

    // RGB Texture for transferring over the network
    m_renderableTexture->m_texSize = texDesc.Width * texDesc.Height * 3;
    m_renderableTexture->m_texData = XCNEW(u8)[m_renderableTexture->m_texSize];
}
#endif
#pragma endregion

void RenderableTexture::PreLoad(ID3DSwapChain* swapChain)
{
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_pRenderTargetResource = gpuSys.CreateRenderTextureResource(swapChain, m_renderTargetType);
    gpuSys.CreateRenderTargetView(m_pRenderTargetResource);
}

void RenderableTexture::Update()
{
#if defined(XCGRAPHICS_DX11)
    XCInput& input = (XCInput&)SystemLocator::GetInstance()->RequestSystem("InputSystem");
    if (input.KeyDown(DIK_R))
    {
        DumpTextureToFile();
    }

    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged->GetResource<ID3D11Texture2D*>(), 0, m_pRenderTargetResource->GetResource<ID3D11Texture2D*>(), 0, m_format);
#endif
}

#pragma region LiveDriveSpecifics
void RenderableTexture::DumpTextureToFile()
{
#if defined(XCGRAPHICS_DX11)
    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged->GetResource<ID3D11Texture2D*>(), 0, m_pRenderTargetResource->GetResource<ID3D11Texture2D*>(), 0, m_format);

    D3D11_MAPPED_SUBRESOURCE subResource = { 0 };

    D3D_TEXTURE2D_DESC texDesc = { 0 };
    m_pRenderTargetResource->GetResource<ID3D11Texture2D*>()->GetDesc(&texDesc);

    m_deviceContext.CopyResource(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), m_pRenderTargetTextureStaged->GetResource<ID3D11Texture2D*>());

    ValidateResult(m_deviceContext.Map(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), 0, D3D11_MAP_READ, 0, &subResource));

    u8* pData = (u8*)subResource.pData;

    DirectX::Image image;
    image.format = m_format;
    image.height = texDesc.Height;
    image.width = texDesc.Width;
    image.pixels = pData;
    image.rowPitch = subResource.RowPitch;
    image.slicePitch = subResource.DepthPitch;

    DirectX::SaveToDDSFile(image, 0, L"Output.dds");

    FILE* fp = fopen("Out.bmp", "wb");
    fwrite(pData, sizeof(u8), texDesc.Height * texDesc.Width * 4, fp);
    fclose(fp);

    m_deviceContext.Unmap(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), 0);
#endif
}

RenderableTexture::RenderedTextureInfo* RenderableTexture::GetRenderToTexture()
{
#if defined(XCGRAPHICS_DX11)
    m_deviceContext.ResolveSubresource(m_pRenderTargetTextureStaged->GetResource<ID3D11Texture2D*>(), 0, m_pRenderTargetResource->GetResource<ID3D11Texture2D*>(), 0, m_format);

    D3D11_MAPPED_SUBRESOURCE subResource = { 0 };

    D3D_TEXTURE2D_DESC texDesc = { 0 };
    m_pRenderTargetResource->GetResource<ID3D11Texture2D*>()->GetDesc(&texDesc);

    m_deviceContext.CopyResource(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), m_pRenderTargetTextureStaged->GetResource<ID3D11Texture2D*>());

    ValidateResult(m_deviceContext.Map(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), 0, D3D11_MAP_READ, 0, &subResource));

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

    m_deviceContext.Unmap(m_pSingleSampledTex->GetResource<ID3D11Texture2D*>(), 0);
#endif

    return m_renderableTexture;
}
#pragma endregion
