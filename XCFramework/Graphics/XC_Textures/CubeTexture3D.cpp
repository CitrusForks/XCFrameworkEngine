/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "Graphics/XC_Textures/CubeTexture3D.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/GPUResourceSystem.h"

CubeTexture3D::CubeTexture3D()
{
    m_resourceType = RESOURCETYPE_CUBETEXTURE3D;
}

CubeTexture3D::~CubeTexture3D()
{
}

void CubeTexture3D::Init(i32 resourceId, std::string userFriendlyName)
{
    Texture2D::Init(resourceId, userFriendlyName);
}

void CubeTexture3D::Load(const void* buffer)
{
    const FBCubeTexture3D* fbTexture3D = (FBCubeTexture3D*)buffer;

    m_userFriendlyName = fbTexture3D->ResourceName()->c_str();
    m_resourcePath = getPlatformPath(fbTexture3D->ResourcePath()->c_str());
    LoadTexture();

    IResource::Load(buffer);
}

void CubeTexture3D::LoadTexture()
{
    Texture2D::LoadTexture();

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

    for (u32 texIndex = 0; texIndex < 6; ++texIndex)
    {
        rtvDesc.Texture2DArray.FirstArraySlice = texIndex;
        ValidateResult(graphicsSystem.GetDevice()->CreateRenderTargetView(m_texture2D, &rtvDesc, &m_cubeMapRTV[texIndex]));
    }
#endif
}

void CubeTexture3D::Destroy()
{
    Texture2D::Destroy();
}