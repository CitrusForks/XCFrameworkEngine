/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/XC_Textures/Texture2D.h"
#include "Graphics/XC_Graphics.h"
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

Texture2D::Texture2D(D3DConstantBuffer* srv)
{
    m_diffuseMapTextureSRV = srv;
    m_resourceType = RESOURCETYPE_TEXTURE2D;
}

Texture2D::~Texture2D()
{
}

void Texture2D::LoadTexture()
{
#if defined(XCGRAPHICS_DX12)
    m_diffuseMapTextureSRVUpload = XCNEW(D3DConstantBuffer)(BUFFERTYPE_SRV);
#endif

    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

    size_t cSize = strlen(m_resourcePath.c_str()) + 1;
    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], m_resourcePath.c_str(), cSize);
    const WCHAR* str = wc.c_str();

#if defined(XCGRAPHICS_DX12)
    ValidateResult(LoadFromDDSFile(str, 0, &m_texMetaData, m_scratchImage));
    m_textureDesc = CD3DX12_RESOURCE_DESC
    (
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        m_scratchImage.GetMetadata().width,
        m_scratchImage.GetMetadata().height,
        1,
        static_cast<UINT16>(m_scratchImage.GetMetadata().mipLevels),
        m_scratchImage.GetMetadata().format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_NONE);

    //Create srv view.
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = m_scratchImage.GetMetadata().format;
    desc.Texture2D.MipLevels = m_scratchImage.GetMetadata().mipLevels;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.ResourceMinLODClamp = 0.0f;

    SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_diffuseMapTextureSRV = heap.CreateShaderResourceView(m_textureDesc, desc);

    const u32 subResCount = m_textureDesc.DepthOrArraySize * m_textureDesc.MipLevels;
    UINT64 intermediateUploadBufferSize = GetRequiredIntermediateSize(m_diffuseMapTextureSRV->m_cbResource, 0, subResCount);
    ValidateResult(graphicsSystem.GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(intermediateUploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_diffuseMapTextureSRVUpload->m_cbResource)));

    //Copy the data to upload heap and then copy it to default heap
    RenderingPool& renderPool = graphicsSystem.GetRenderingPool();
    renderPool.RequestResourceDeviceContext(this);
    WaitResourceUpdate();

#elif defined(XCGRAPHICS_DX11)
    SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_diffuseMapTextureSRV = heap.CreateShaderResourceView();

    ValidateResult(LoadFromDDSFile(str, 0, &m_texMetaData, m_scratchImage));
    ValidateResult(CreateShaderResourceView(graphicsSystem.GetDevice(), m_scratchImage.GetImages(), m_scratchImage.GetImageCount(), m_scratchImage.GetMetadata(), &m_diffuseMapTextureSRV->m_cbResource));

    m_resourceUpdated = true;
#endif

    Logger("[TEXTURE 2D] Texture %s Loaded", wc.c_str());
}

void Texture2D::RenderContextCallback(RenderContext& renderContext)
{
#if defined(XCGRAPHICS_DX12)
    ID3DDeviceContext& context = renderContext.GetDeviceContext();
    
    D3D12_SUBRESOURCE_DATA textureData[6];  //Max 6 subResources counts required by cube maps.

    for (u32 imageIndex = 0; imageIndex < m_scratchImage.GetImageCount(); ++imageIndex)
    {
        const DirectX::Image* image = m_scratchImage.GetImage(0, imageIndex, 0);

        textureData[imageIndex].pData = image->pixels;
        textureData[imageIndex].RowPitch = image->rowPitch;
        textureData[imageIndex].SlicePitch = image->slicePitch;
    }

    const u32 subResCount = m_scratchImage.GetMetadata().arraySize;
    UpdateSubresources(&context, m_diffuseMapTextureSRV->m_cbResource, m_diffuseMapTextureSRVUpload->m_cbResource, 0, 0, subResCount, &textureData[0]);

    context.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_diffuseMapTextureSRV->m_cbResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
#endif

    IResource::RenderContextCallback(renderContext);
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
        m_diffuseMapTextureSRV->Release();
        SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
        heap.DestroyBuffer(m_diffuseMapTextureSRV);
        m_diffuseMapTextureSRV = nullptr;
    }

#if defined(XCGRAPHICS_DX12)
    if (m_diffuseMapTextureSRVUpload)
    {
        m_diffuseMapTextureSRVUpload->Release();
        XCDELETE(m_diffuseMapTextureSRVUpload);
        m_diffuseMapTextureSRVUpload = nullptr;
    }
#endif
}

void Texture2D::Destroy()
{
    IResource::Destroy();
    
    Unload();
}
