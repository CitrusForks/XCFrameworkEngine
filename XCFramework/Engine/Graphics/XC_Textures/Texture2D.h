/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"

#if defined(XCGRAPHICS_DX12)
#include "Libs/DirectXTex/DirectXTex/DirectXTex.h"
#elif defined(XCGRAPHICS_DX11)
#include "Libs/DDSTextureLoader.h"
#endif

class Texture2D : public IResource
{
public:
    DECLARE_OBJECT_CREATION(Texture2D)

    Texture2D();
    Texture2D(D3DShaderResourceView* srv);

    virtual ~Texture2D();

    virtual void                 Load(std::string resourcePath);
    virtual void                 Load(const void* fbTexture2D);
    virtual void                 Destroy();
    
    void                         RenderContextCallback(RenderContext& renderContext);
    void                         loadTexture();
    D3DShaderResourceView*       getTextureResource() { return m_diffuseMapTextureSRV; }
    void                         setSRV(D3DShaderResourceView* srv) { m_diffuseMapTextureSRV = srv; }
    XCMatrix4*                   getTextureCoordinateMatrix() { return &m_textureCoordinateMatrix; }
    void                         setTextureCoordinateMatrix(XCMatrix4 matrix) { m_textureCoordinateMatrix = matrix; }

protected:
    D3DShaderResourceView*       m_diffuseMapTextureSRV;
    XCMatrix4                    m_textureCoordinateMatrix;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*           m_sharedCB;                    //Shared ShaderResourceView ref'ed from SharedDescriptorHeap.
    D3DShaderResourceView*       m_diffuseMapTextureSRVUpload;
    DirectX::TexMetadata         m_texMetaData;
    DirectX::ScratchImage        m_scratchImage;
    CD3DX12_RESOURCE_DESC        m_textureDesc;
    D3D12_CPU_DESCRIPTOR_HANDLE  m_srvHandle;
#endif
};