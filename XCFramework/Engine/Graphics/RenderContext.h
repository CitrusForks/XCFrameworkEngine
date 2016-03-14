/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"

class XC_Graphics;

class RenderContext
{
public:
    RenderContext();

    void                        Init(ID3DDevice* device, XC_ShaderManager& shaderMgr, bool clearStateOnBegin, bool clearStateOnFinish);
    void                        BeginRender();
    void                        FinishRender();
    void                        ReleaseCommandList();
    void                        Destroy();

    ID3DDeviceContext&          GetDeviceContext() { return *m_deviceContext; }

#if defined(XCGRAPHICS_DX12)
    ID3DCommandAllocator*       GetCommandAllocator() { return m_commandAllocator; }
#elif defined(XCGRAPHICS_DX11)
    ID3DCommandList&            GetCommandList() { return *m_commandList; }
#endif

    void                        SetRasterizerState(ERasterizer_Type type);
    void                        ApplyShader(SHADERTYPE shaderType);
    XC_ShaderManager&           GetShaderManagerSystem() { return *m_shaderManager; }

private:
    XC_ShaderManager*            m_shaderManager;
    ID3DDeviceContext*           m_deviceContext;

#if defined(XCGRAPHICS_DX12)
    ID3DCommandAllocator*        m_commandAllocator;
#elif defined(XCGRAPHICS_DX11)
    ID3DCommandList*             m_commandList;
#endif

    XC_Graphics*                 m_graphicsSystem;
    bool                         m_clearStateOnBegin;
    bool                         m_clearStateOnFinish;
    XCVec4                       m_clearColor;
};