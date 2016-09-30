/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_GL)
#include "Base/System/ISystem.h"
#include "Graphics/XCGraphics.h"

class XCGraphicsGL : public XCGraphics
{
public:
    DECLARE_OBJECT_CREATION(XCGraphicsGL)

    XCGraphicsGL(void);
    virtual ~XCGraphicsGL(void);
    
    void                            Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    void                            Destroy();
    void                            Update(f32 dt);
    void                            BeginScene();
    void                            EndScene();
    void                            BeginSecondaryScene();
    void                            EndSecondaryScene();
    
    ID3DDevice*                     GetDevice()         { return m_pD3DDevice; }
    ID3DDeviceContext*              GetDeviceContext() { return nullptr; }

    void                            GoFullscreen(bool go);

    void                            OnResize(i32 _width, i32 _height);

    void                            TurnOffZ();
    void                            TurnOnZ();

    void                            SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);
    void                            ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList) {}

protected:
    void                            SetupPipeline();
    void                            SetupDevice();
    void                            SetupSwapChain();
    void                            SetupRenderTargets();
    void                            SetupDepthStencilBuffer();
    void                            SetupDepthStencilStates();
    void                            SetupDepthView();
    void                            SetupViewPort();
    void                            SetupShadersAndRenderPool();

    void                            PresentRenderTarget(ID3D12GraphicsCommandList* cmdList) {}

private:
#if defined(DEBUG_GRAPHICS_PIPELINE)
    enum VAO_Ids
    {
        VAO_Triangle,
        Max_VAOs
    };

    enum Buffer_Ids
    {
        ArrayBuffer,
        Max_Buffers
    };

    enum Attribute_Ids
    {
        VertexPosition = 0
    };

    GLuint m_VAOs[Max_VAOs];
    GLuint m_buffers[Max_Buffers];

    static const GLuint NUM_VERTICES = 6;
    std::vector<GLfloat> m_triangleVertices;
#endif
};

#endif