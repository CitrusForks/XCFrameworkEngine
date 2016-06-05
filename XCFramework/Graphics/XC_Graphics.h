/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Graphics/RenderingPool.h"
#include "Graphics/XC_Textures/RenderableTexture.h"
#include "Graphics/RenderTargetTypes.h"

class XC_Graphics : public ISystem
{
public:
    XC_Graphics(void);
    virtual ~XC_Graphics(void);
    
    virtual  void               InitGraphicsWindow(HWND _mainWnd, int _width, int _height, bool _enable4xMsaa);
    ID3DDevice*                 GetDevice()         { return m_pD3DDevice; }

    virtual ID3DDeviceContext*      GetDeviceContext() = 0;
    virtual ID3DCommandAllocator*   GetDeviceCommandAllocator() = 0;
    virtual ID3DResource*           GetCurrentFrameRenderTarget() = 0;
    virtual CPU_DESCRIPTOR_HANDLE   GetRTVCPUDescHandler() = 0;
    virtual CPU_DESCRIPTOR_HANDLE   GetDSVCPUDescHandler() = 0;
    virtual ID3DCommandQueue*       GetCommandQueue() = 0;

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

    bool                        IsSecondaryDrawCall() { return m_secondaryDrawCall; }
    void                        SetSecondaryDrawCall(bool isSecondary) { m_secondaryDrawCall = isSecondary; }
    
    virtual void                OnResize(int _width, int _height);
    virtual void                SetClearColor(XCVec4 color)           { m_clearColor = color; }

    virtual void                TurnOffZ();
    virtual void                TurnOnZ();
    unsigned int                GetMsaaQuality() { return m_4xMsaaQuality; }

    virtual ID3D12PipelineState*    GetPipelineState() { return nullptr; }

    XC_ShaderManager&               GetShaderManagerSystem() { return *m_XCShaderSystem; }
    RenderingPool&                  GetRenderingPool() { return *m_renderingPool; }

#if defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_GNM)
    RenderableTexture&              GetRenderTexture(RenderTargetsType type) { return *m_renderTargets[type]; }
#endif

#if defined(XCGRAPHICS_DX11) 
    virtual ID3DDepthStencilView&   GetDepthStencilView(RenderTargetsType type) = 0;
#endif

    virtual void                SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);
    D3D_VIEWPORT&               GetViewPort(RenderTargetsType type) { return m_ScreenViewPort[type]; }
    D3D_RECT&                   GetScissorRect() { return m_scissorRect; }
    void                        PresentRenderTarget(ID3D12GraphicsCommandList* cmdList) {}
    virtual void                ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList){}
    
#if defined(XCGRAPHICS_GNM)
    virtual sce::Gnm::Sampler&  GetDefaultSampler() = 0;
#endif
    virtual int                 GetUserId() { return 0; }

    std::string                 GetDefaultWindowTitle();
    void                        SetWindowTitle(std::string value);

protected:
    ID3DDevice*                 m_pD3DDevice;

    int                         m_ClientWidth;
    int                         m_ClientHeight;
    bool                        m_Enable4xMsaa;
    
    HWND                        m_hMainWnd;
    bool                        m_initDone;

    XCVec4                      m_clearColor;

#if defined(XCGRAPHICS_DX11) || defined(XCGRAPHICS_GNM)
    RenderableTexture*          m_renderTargets[RENDERTARGET_MAX];
#endif
    bool                        m_secondaryDrawCall;
    unsigned int                m_4xMsaaQuality;
    
    XC_ShaderManager*           m_XCShaderSystem;
    RenderingPool*              m_renderingPool;
    D3D_VIEWPORT                m_ScreenViewPort[RENDERTARGET_MAX];
    D3D_RECT                    m_scissorRect;
};