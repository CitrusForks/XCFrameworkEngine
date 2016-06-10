/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Resource/IResource.h"
#include "Graphics/D3DConstantBuffer.h"

#include "Libs/DirectXTex/DirectXTex/DirectXTex.h"

class Texture2D : public IResource
{
public:
    DECLARE_OBJECT_CREATION(Texture2D)

    Texture2D();
    Texture2D(D3DConstantBuffer* srv);

    virtual ~Texture2D();

    virtual void                 Load(std::string resourcePath) override;
    virtual void                 Load(const void* fbTexture2D)  override;
    virtual void                 UpdateState();
    virtual void                 Unload()                       override;
    virtual void                 Destroy()                      override;
    
    void                         RenderContextCallback(RenderContext& renderContext);
    void                         LoadTexture();
    
    D3DConstantBuffer*           GetTextureResource()               { return m_diffuseMapTextureSRV; }
    void                         SetSRV(D3DConstantBuffer* srv)     { m_diffuseMapTextureSRV = srv; }
    
    XCMatrix4*                   GetTextureCoordinateMatrix() { return &m_textureCoordinateMatrix; }
    void                         SetTextureCoordinateMatrix(XCMatrix4 matrix) { m_textureCoordinateMatrix = matrix; }

protected:

    D3DConstantBuffer*           m_diffuseMapTextureSRV;
    XCMatrix4                    m_textureCoordinateMatrix;

    DirectX::TexMetadata         m_texMetaData;
    DirectX::ScratchImage        m_scratchImage;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*           m_diffuseMapTextureSRVUpload;
    CD3DX12_RESOURCE_DESC        m_textureDesc;
    D3D12_CPU_DESCRIPTOR_HANDLE  m_srvHandle;
#endif
};