/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once
#ifndef __TERRAINMULTITEX__
#define __TERRAINMULTITEX__

#include "Graphics/XC_Shaders/src/LightTexture.h"

class TerrainMultiTex :
    public LightTextureShader
{
public:
    static const unsigned int NbOfDescriptors = 10;

    TerrainMultiTex(ID3DDevice& device);
    virtual ~TerrainMultiTex();

    virtual void                     loadShader();
    virtual void                     createBufferConstants();

    void                             setTexture2D1(ID3DDeviceContext& context,D3DShaderResourceView* tex);
    void                             setTexture2D2(ID3DDeviceContext& context,D3DShaderResourceView* tex);
    void                             setTexture2DBlend(ID3DDeviceContext& context, D3DShaderResourceView* tex);

    void                             applyShader(ID3DDeviceContext& context);
    void                             destroy();
    void                             reset(ID3DDeviceContext& context);

protected:
    D3DShaderResourceView*        m_pTexture2DDiffuseMap1;  //Mapped with gDiffuseMap
    D3DShaderResourceView*        m_pTexture2DDiffuseMap2;  //Mapped with gDiffuseMap
    D3DShaderResourceView*        m_pTexture2DBlendMap;  //Mapped with gDiffuseMap
};

#endif