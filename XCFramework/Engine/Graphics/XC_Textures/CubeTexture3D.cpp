/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Engine/Graphics/XC_Textures/CubeTexture3D.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Graphics/SharedDescriptorHeap.h"

CubeTexture3D::CubeTexture3D()
{
    m_resourceType = RESOURCETYPE_CUBETEXTURE3D;
    m_textureCoordinateMatrix = XMMatrixIdentity();

#if defined(XCGRAPHICS_DX11)
    m_texture2DDesc = { 0 };
    m_texture2D = nullptr;

    for (unsigned int index = 0; index < 6; ++index)
    {
        m_cubeMapRTV[index] = nullptr;
    }
#endif
}

CubeTexture3D::~CubeTexture3D()
{
}

void CubeTexture3D::Init(int resourceId, std::string userFriendlyName)
{
    Texture2D::Init(resourceId, userFriendlyName);

    /*XC_GraphicsDx11& graphicSys = SystemLocator::GetInstance()->RequestSystem<XC_GraphicsDx11>("GraphicsSystem");

    m_texture2DDesc.Width = CUBE_MAP_SIZE;
    m_texture2DDesc.Height = CUBE_MAP_SIZE;
    m_texture2DDesc.MipLevels = 0;
    m_texture2DDesc.ArraySize = 6;
    m_texture2DDesc.SampleDesc.Count = 1;
    m_texture2DDesc.SampleDesc.Quality = 0;
    m_texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
    m_texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    m_texture2DDesc.CPUAccessFlags = 0;
    m_texture2DDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

    //Create the 6 Texture2D
    ValidateResult(graphicSys.GetDevice()->CreateTexture2D(&m_texture2DDesc, 0, &m_texture2D));
    
    CreateRenderTargets();*/
}

void CubeTexture3D::Load(const void* buffer)
{
    const FBCubeTexture3D* fbTexture3D = (FBCubeTexture3D*)buffer;

    m_userFriendlyName = fbTexture3D->ResourceName()->c_str();
    m_resourcePath = getPlatformPath(fbTexture3D->ResourcePath()->c_str());
    loadTexture();

    IResource::Load(buffer);
}

void CubeTexture3D::loadTexture()
{
    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");

#if defined(XCGRAPHICS_DX12)
    size_t cSize = strlen(m_resourcePath.c_str()) + 1;
    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], m_resourcePath.c_str(), cSize);
    const WCHAR* str = wc.c_str();

    ValidateResult(LoadFromDDSFile(str, 0, &m_texMetaData, m_scratchImage));
    m_textureDesc = CD3DX12_RESOURCE_DESC
        (
            D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            0,
            m_scratchImage.GetMetadata().width,
            m_scratchImage.GetMetadata().height,
            m_scratchImage.GetMetadata().arraySize,
            static_cast<UINT16>(m_scratchImage.GetMetadata().mipLevels),
            m_scratchImage.GetMetadata().format,
            1,
            0,
            D3D12_TEXTURE_LAYOUT_UNKNOWN,
            D3D12_RESOURCE_FLAG_NONE);

    //Create srv view.
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = m_scratchImage.GetMetadata().format;

    if (m_scratchImage.GetMetadata().IsCubemap())
    {
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        desc.TextureCube.MipLevels = m_scratchImage.GetMetadata().mipLevels;
        desc.TextureCube.MostDetailedMip = 0;
        desc.TextureCube.ResourceMinLODClamp = 0.0f;
    }
    else
    {
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = m_scratchImage.GetMetadata().mipLevels;
        desc.Texture2D.MostDetailedMip = 0;
        desc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    SharedDescriptorHeap& heap = SystemLocator::GetInstance()->RequestSystem<SharedDescriptorHeap>("SharedDescriptorHeap");
    m_diffuseMapTextureSRV = heap.CreateShaderResourceView(m_textureDesc, desc);

    const unsigned int subResCount = m_textureDesc.DepthOrArraySize * m_textureDesc.MipLevels;
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
    Texture2D::loadTexture();
#endif

    Logger("[TEXTURE 3D] Texture Loaded");
}

void CubeTexture3D::CreateRenderTargets()
{
#if defined(XCGRAPHICS_DX11)
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

    //Create render targets for all the texture
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

    rtvDesc.Format = m_texture2DDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.MipSlice = 0;
    rtvDesc.Texture2DArray.ArraySize = 1;

    for (unsigned int texIndex = 0; texIndex < 6; ++texIndex)
    {
        rtvDesc.Texture2DArray.FirstArraySlice = texIndex;
        ValidateResult(graphicsSystem.GetDevice()->CreateRenderTargetView(m_texture2D, &rtvDesc, &m_cubeMapRTV[texIndex]));
    }
#endif
}

void CubeTexture3D::Destroy()
{
#if defined(XCGRAPHICS_DX11)
    ReleaseCOM(m_texture2D);
    for (unsigned int index = 0; index < 6; ++index)
    {
        if (m_cubeMapRTV[index] != nullptr)
        {
            ReleaseCOM(m_cubeMapRTV[index]);
        }
    }
#endif
    Texture2D::Destroy();
}