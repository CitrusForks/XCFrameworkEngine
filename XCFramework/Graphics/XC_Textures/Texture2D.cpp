/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/XC_Textures/Texture2D.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/GPUResource.h"
#include "Graphics/GPUResourceSystem.h"
#include "Graphics/SharedDescriptorHeap.h"

#if defined(XCGRAPHICS_GNM)
#include "Engine/Memory/MemorySystemOrbis.h"
#include "Graphics/XC_Textures/GnfTextureLoader.h"
//#define USE_DEFAULT_TEXTURE 1
#endif

Texture2D::Texture2D()
{
    m_resourceType = RESOURCETYPE_TEXTURE2D;
    m_diffuseMapTextureSRV = nullptr;
}

Texture2D::Texture2D(GPUResource* srv)
{
    m_diffuseMapTextureSRV = srv;
    m_resourceType = RESOURCETYPE_TEXTURE2D;
}

Texture2D::~Texture2D()
{
}

void Texture2D::LoadTexture()
{
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

    size_t cSize = strlen(m_resourcePath.c_str()) + 1;
    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], m_resourcePath.c_str(), cSize);
    const WCHAR* str = wc.c_str();

    ValidateResult(LoadFromDDSFile(str, 0, &m_texMetaData, m_scratchImage));

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& descHeap = (SharedDescriptorHeap&) SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_diffuseMapTextureSRVUpload = descHeap.AllocateGPUResource(GPUResourceType_SRV, 0);

    CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC
    (
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,                         //Dimensions
        0,                                                          //alignment
        m_scratchImage.GetMetadata().width,                         //width
        m_scratchImage.GetMetadata().height,                        //height
        m_scratchImage.GetMetadata().arraySize,                     //arraysize
        static_cast<UINT16>(m_scratchImage.GetMetadata().mipLevels),//miplevels
        m_scratchImage.GetMetadata().format,                        //format
        1,                                                          //sample count
        0,                                                          //sample quality
        D3D12_TEXTURE_LAYOUT_UNKNOWN,                               //layout
        D3D12_RESOURCE_FLAG_NONE);                                  //flags

    //Create srv view.
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = m_scratchImage.GetMetadata().IsCubemap() ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = m_scratchImage.GetMetadata().format;
    desc.Texture2D.MipLevels = m_scratchImage.GetMetadata().mipLevels;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.ResourceMinLODClamp = 0.0f;

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_diffuseMapTextureSRV = gpuSys.CreateTextureResource(texDesc, nullptr);
    gpuSys.CreateShaderResourceView(desc, m_diffuseMapTextureSRV);

    const u32 subResCount = texDesc.DepthOrArraySize * texDesc.MipLevels;
    UINT64 intermediateUploadBufferSize = GetRequiredIntermediateSize(m_diffuseMapTextureSRV->GetResource<ID3D12Resource*>(), 0, subResCount);
    ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(intermediateUploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_diffuseMapTextureSRVUpload->GetPointerToResource<ID3D12Resource*>())));

    //Copy the data to upload heap and then copy it to default heap
    RenderingPool& renderPool = graphicsSystem.GetRenderingPool();
    renderPool.RequestResourceDeviceContext(this);
    WaitResourceUpdate();

#elif defined(XCGRAPHICS_DX11)
    D3D11_TEXTURE2D_DESC texDesc = {};
    ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

    texDesc.Width = m_scratchImage.GetMetadata().width,
    texDesc.Height = m_scratchImage.GetMetadata().height;
    texDesc.ArraySize = m_scratchImage.GetMetadata().arraySize;
    texDesc.MipLevels = m_scratchImage.GetMetadata().mipLevels;
    texDesc.Format = m_scratchImage.GetMetadata().format;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = m_scratchImage.GetMetadata().IsCubemap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = m_scratchImage.GetMetadata().IsCubemap() ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
    desc.Format = m_scratchImage.GetMetadata().format;
    desc.Texture2D.MipLevels = m_scratchImage.GetMetadata().mipLevels;
    desc.Texture2D.MostDetailedMip = 0;

    D3D11_SUBRESOURCE_DATA textureData[6];  //Max 6 subResources counts required by cube maps.

    for (u32 imageIndex = 0; imageIndex < m_scratchImage.GetImageCount(); ++imageIndex)
    {
        const DirectX::Image* image = m_scratchImage.GetImage(0, imageIndex, 0);

        textureData[imageIndex].pSysMem = image->pixels;
        textureData[imageIndex].SysMemPitch = image->rowPitch;
        textureData[imageIndex].SysMemSlicePitch = image->slicePitch;
    }

    GPUResourceSystem& gpuSystem = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_diffuseMapTextureSRV = gpuSystem.CreateTextureResource(texDesc, textureData);
    gpuSystem.CreateShaderResourceView(desc, m_diffuseMapTextureSRV);

    m_resourceUpdated = true;
#endif

    Logger("[TEXTURE 2D] Texture %s Loaded", wc.c_str());
}

void Texture2D::RenderContextCallback(ID3DDeviceContext& context)
{
#if defined(XCGRAPHICS_DX12)
    D3D12_SUBRESOURCE_DATA textureData[6];  //Max 6 subResources counts required by cube maps.

    for (u32 imageIndex = 0; imageIndex < m_scratchImage.GetImageCount(); ++imageIndex)
    {
        const DirectX::Image* image = m_scratchImage.GetImage(0, imageIndex, 0);

        textureData[imageIndex].pData = image->pixels;
        textureData[imageIndex].RowPitch = image->rowPitch;
        textureData[imageIndex].SlicePitch = image->slicePitch;
    }

    const u32 subResCount = m_scratchImage.GetMetadata().arraySize;
    UpdateSubresources(&context, m_diffuseMapTextureSRV->GetResource<ID3DResource*>(), m_diffuseMapTextureSRVUpload->GetResource<ID3DResource*>(), 0, 0, subResCount, &textureData[0]);

    context.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_diffuseMapTextureSRV->GetResource<ID3DResource*>(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
#endif

    IResource::RenderContextCallback(context);
}

void Texture2D::Load(std::string resourcePath)
{
    m_userFriendlyName = "dynamically_spawned_texture2D";
    m_resourcePath = resourcePath;
    LoadTexture();

    IResource::Load(nullptr);
}

void Texture2D::Load(const void* buffer)
{
    const FBTexture2D* fbTexture2D = (FBTexture2D*) buffer;

    m_userFriendlyName = fbTexture2D->ResourceName()->c_str();
    m_resourcePath = getPlatformPath(fbTexture2D->ResourcePath()->c_str());
    LoadTexture();

    IResource::Load(buffer);
}

void Texture2D::UpdateState()
{
    if (m_resourceUpdated)
    {
        m_resourceState = IResource::ResourceState_Loaded;
    }

    IResource::UpdateState();
}

void Texture2D::Unload()
{
    if (m_diffuseMapTextureSRV)
    {
        m_diffuseMapTextureSRV->Destroy();
        GPUResourceSystem& gpuSystem = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
        gpuSystem.DestroyResource(m_diffuseMapTextureSRV);
        m_diffuseMapTextureSRV = nullptr;
    }

#if defined(XCGRAPHICS_DX12)
    if (m_diffuseMapTextureSRVUpload)
    {
        m_diffuseMapTextureSRVUpload->Destroy();
        GPUResourceSystem& gpuSystem = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
        gpuSystem.DestroyResource(m_diffuseMapTextureSRVUpload);
        m_diffuseMapTextureSRVUpload = nullptr;
    }
#endif
}

void Texture2D::Destroy()
{
    IResource::Destroy();
    
    Unload();
}
