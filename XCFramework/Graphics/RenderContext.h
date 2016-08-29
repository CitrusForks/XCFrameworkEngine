/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Assets/Packages/PackageConsts.h"

#include "Graphics/RenderTargetTypes.h"
#include "Graphics/XC_Shaders/GlobalShaderData.h"

class XC_Graphics;
class XC_ShaderContainer;
class IShader;

class RenderContext
{
public:
    RenderContext();

    void                                Init(ID3DDevice* device, XC_ShaderContainer* shaderMgr, bool clearStateOnBegin, bool clearStateOnFinish);
    void                                BeginRender(RenderTargetsType targetType);
    void                                FinishRender();
    void                                Destroy();

    ID3DDeviceContext&                  GetDeviceContext() { return *m_deviceContext; }

    void                                ApplyShader(ShaderType shaderType, RasterType type = RasterType_FillSolid);
    IShader*                            GetShader(ShaderType shaderType);

    GlobalShaderData&                   GetGlobalShaderData();

    void                                DrawNonIndexed(ID3DDeviceContext& context, u32 vertexCount);
    void                                DrawIndexedInstanced(ID3DDeviceContext& context, u32 _indexCount, void* indexGpuAddr = nullptr, u32 instanceCount = 1);

protected:
    void                                ReleaseCommandList();

private:
    XC_ShaderContainer*                   m_shaderContainer;
    ID3DDeviceContext*                  m_deviceContext;

#if defined(XCGRAPHICS_DX12)
    ID3DCommandAllocator*               m_commandAllocator;
#elif defined(XCGRAPHICS_DX11)
    ID3DCommandList*                    m_commandList;
#endif

    XC_Graphics*                        m_graphicsSystem;
    XCVec4                              m_clearColor;

    bool                                m_clearStateOnBegin;
    bool                                m_clearStateOnFinish;
};