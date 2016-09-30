/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#ifndef __SOLIDCOLOR__
#define __SOLIDCOLOR__

#include "Engine/Graphics/XC_Shaders/src/IShader.h"

//Shader Collector and list
//Maintains and Maps shaders with their param list
class SolidColorShader : public IShader
{
public:
    static const unsigned int NbOfDescriptors = 3;

    SolidColorShader(ID3DDevice& device)
        : IShader(device)
    { }

    void         createBufferConstants();
    void         destroy();

    void         loadShader();
    void         setWVP(ID3DDeviceContext& context, cbWVP wvp);

#if defined(XCGRAPHICS_DX12)
    void         setWVP(ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& wvpGPUHandle);
#endif

    void         applyShader(ID3DDeviceContext& context);
    void         reset(ID3DDeviceContext& context);

private:
    D3DConstantBuffer*  m_pPerObject;
};

#endif