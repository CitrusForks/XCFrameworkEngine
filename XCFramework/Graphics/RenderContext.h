/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Assets/Packages/PackageConsts.h"

#include "Graphics/RenderTargetTypes.h"
#include "Graphics/XCShaders/GlobalShaderData.h"

class XCGraphics;
class XCShaderContainer;
class IShader;

class RenderContext
{
public:
    RenderContext();

    void                                Init(XCShaderContainer* shaderMgr);
    void                                Reset();
    void                                BeginRender(std::vector<RenderTargetsType>& targetType);
    void                                FinishRender();
    void                                Destroy();

    ID3DDeviceContext&                  GetDeviceContext() { return *m_deviceContext; }

    void                                ApplyShader(ShaderType shaderType, RasterType type = RasterType_FillSolid);
    IShader*                            GetShader(ShaderType shaderType);

    GlobalShaderData&                   GetGlobalShaderData();

    void                                DrawNonIndexed(u32 vertexCount);
    void                                DrawIndexedInstanced(u32 _indexCount, void* indexGpuAddr = nullptr, u32 instanceCount = 1);

private:
    XCShaderContainer*                  m_shaderContainer;
    ID3DDeviceContext*                  m_deviceContext;

#if defined(XCGRAPHICS_DX12)
    ID3DCommandAllocator*               m_commandAllocator;
#endif

    XCGraphics*                         m_graphicsSystem;
    XCVec4                              m_clearColor;
};