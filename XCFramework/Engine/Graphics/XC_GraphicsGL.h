/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_GL)
#include "Engine/System/ISystem.h"
#include "Engine/Graphics/XC_Graphics.h"

class XC_GraphicsGL : public XC_Graphics
{
public:
    DECLARE_OBJECT_CREATION(XC_GraphicsGL)

    XC_GraphicsGL(void);
    virtual ~XC_GraphicsGL(void);
    
    virtual  void               InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa);
    ID3DDevice*                 GetDevice()         { return m_pD3DDevice; }

    virtual ID3DDeviceContext*      GetDeviceContext() { return nullptr; }
    virtual ID3DCommandAllocator*   GetDeviceCommandAllocator() { return nullptr; }
    virtual ID3DResource*           GetCurrentFrameRenderTarget() { return nullptr; }
    virtual CPU_DESCRIPTOR_HANDLE   GetRTVCPUDescHandler() { return nullptr; }
    virtual CPU_DESCRIPTOR_HANDLE   GetDSVCPUDescHandler() { return nullptr; }
    virtual ID3DCommandQueue*       GetCommandQueue() { return nullptr; };

    virtual void                SetupPipeline();
    virtual void                SetupDevice();
    virtual void                SetupSwapChain();
    virtual void                SetupRenderTargets();
    virtual void                SetupDepthStencilBuffer();
    virtual void                SetupDepthStencilStates();
    virtual void                SetupDepthView();
    virtual void                SetupViewPort();
    virtual void                SetupShadersAndRenderPool();

    virtual void                Destroy();
    virtual void                Update(float dt);
    virtual void                BeginScene();
    virtual void                EndScene();
    virtual void                BeginSecondaryScene();
    virtual void                EndSecondaryScene();

    virtual void                GoFullscreen(bool go);

    bool                        isSecondaryDrawCall() { return m_secondaryDrawCall; }
    void                        setSecondaryDrawCall(bool isSecondary) { m_secondaryDrawCall = isSecondary; }
    
    virtual void                OnResize(int _width, int _height);
    virtual void                SetClearColor(XCVec4 color)           { m_clearColor = color; }

    virtual void                TurnOffZ();
    virtual void                TurnOnZ();
    unsigned int                getMsaaQuality() { return m_4xMsaaQuality; }

    virtual ID3D12PipelineState* GetPipelineState() { return nullptr; }

    XC_ShaderManager&           GetShaderManagerSystem() { return *m_XCShaderSystem; }
    RenderingPool&              GetRenderingPool() { return *m_renderingPool; }

    virtual void                SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);
    D3D_VIEWPORT&               GetViewPort(RenderTargetsType type) { return m_ScreenViewPort[type]; }
    D3D_RECT&                   GetScissorRect() { return m_scissorRect; }
    void                        PresentRenderTarget(ID3D12GraphicsCommandList* cmdList) {}
    virtual void                ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList){}
    
    virtual int                 GetUserId() { return 0; }

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