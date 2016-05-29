/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Textures/Texture2D.h"

class CubeTexture3D : public Texture2D
{
public:
    DECLARE_OBJECT_CREATION(CubeTexture3D)

    static const int    CUBE_MAP_SIZE = 256;

    CubeTexture3D();
    ~CubeTexture3D();

    virtual void        Init(int resourceId, std::string userFriendlyName) override;
    virtual void        Load(const void* buffer)                           override;
    virtual void        Destroy()                                          override;

    void                LoadTexture();
    void                CreateRenderTargets();

private:
#if defined(XCGRAPHICS_DX11)
    D3D11_TEXTURECUBE_FACE              m_textureCubeFace;
    D3D_TEXTURE2D_DESC                  m_texture2DDesc;
    ID3DTexture2D*                      m_texture2D;
    ID3DRenderTargetView*               m_cubeMapRTV[6];
#endif
};