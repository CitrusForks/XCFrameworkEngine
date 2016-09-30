/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#ifndef __DEFAULT__
#define __DEFAULT__

#include "Engine/Graphics/XC_Shaders/src/IShader.h"

#if defined(XC_GRAPHICS_GNM)
#include "Assets/Shaders/Default/std_cbuffer.h"
#endif

class DefaultShader : public IShader
{
public:
    static const unsigned int NbOfDescriptors = 3;

    DefaultShader(ID3DDevice& device)
        : IShader(device)
    { }

    void         createBufferConstants();
    void         loadShader();
    void         reset(ID3DDeviceContext& context);
    void         setWVP(ID3DDeviceContext& context, cbWVP& wvp);
    void         setTex2D(ID3DDeviceContext& context, D3DShaderResourceView* tex);

private:

#if defined(XC_GRAPHICS_GNM)
    sce::Gnm::Buffer    m_cbWvp;
#endif

    cbWVP*              m_wvp;
};

#endif