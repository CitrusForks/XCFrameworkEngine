/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"

#include "Libs/DirectXTex/DirectXTex.h"

class GPUResource;

class Texture2D : public IResource
{
public:
    DECLARE_OBJECT_CREATION(Texture2D)

    Texture2D();
    Texture2D(GPUResource* srv);

    virtual ~Texture2D();

    virtual void                 Load(std::string resourcePath) override;
    virtual void                 Load(const void* fbTexture2D)  override;
    virtual void                 UpdateState();
    virtual void                 Unload()                       override;
    virtual void                 Destroy()                      override;
    
    //RenderContextCallback is a callback from main thread, which is required for dx12 resources to move the data from cpu to gpu memory.
    void                         RenderContextCallback(ID3DDeviceContext& renderContext);
    void                         LoadTexture();
    
    GPUResource*                 GetTextureResource()                       { return m_diffuseMapTextureSRV; }
    void                         SetTextureResource(GPUResource* srv)       { m_diffuseMapTextureSRV = srv; }
    
    XCMatrix4*                   GetTextureCoordinateMatrix() { return &m_textureCoordinateMatrix; }
    void                         SetTextureCoordinateMatrix(const XCMatrix4& matrix) { m_textureCoordinateMatrix = matrix; }

protected:

    GPUResource*                 m_diffuseMapTextureSRV;
    XCMatrix4                    m_textureCoordinateMatrix;

    DirectX::TexMetadata         m_texMetaData;
    DirectX::ScratchImage        m_scratchImage;

#if defined(XCGRAPHICS_DX12)
    GPUResource*                 m_diffuseMapTextureSRVUpload;
    D3D12_CPU_DESCRIPTOR_HANDLE  m_srvHandle;
#endif
};