/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "Graphics/XC_Shaders/XC_ShaderContainer.h"
#include "Graphics/RenderingPool.h"
#include "Graphics/XC_Textures/RenderableTexture.h"
#include "Graphics/RenderTargetTypes.h"

class GPUResourceSystem;
class SharedDescriptorHeap;

class XC_Graphics : public ISystem
{
public:
    XC_Graphics(void);
    virtual ~XC_Graphics(void);
    
    virtual void                    Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    virtual void                    BeginScene();
    virtual void                    EndScene();
    virtual void                    BeginSecondaryScene();
    virtual void                    EndSecondaryScene();
    virtual void                    Update(f32 dt);
    virtual void                    Destroy();

    virtual u32                     GetCurrentRTVFrameIndex() = 0;
    virtual ID3DDeviceContext*      GetDeviceContext() = 0;
    virtual GPUResource*            GetDepthStencilView(RenderTargetsType type) { return m_depthStencilResource[type]; };
    virtual ID3DPipelineState*      GetPipelineState() { return nullptr; }

    virtual void                    GoFullscreen(bool go);
    virtual void                    OnResize(i32 _width, i32 _height);

    virtual void                    SetClearColor(const XCVec4& color) { m_clearColor = color; }

    virtual void                    TurnOffZ();
    virtual void                    TurnOnZ();
    virtual void                    SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);
    virtual void                    ClearRTVAndDSV(ID3DDeviceContext* context, RenderTargetsType type) {}

    ID3DDevice*                     GetDevice() { return m_pD3DDevice; }
    XC_ShaderContainer&             GetShaderContainer() { return *m_XCShaderSystem; }
    RenderingPool&                  GetRenderingPool() { return *m_renderingPool; }
    RenderableTexture&              GetRenderTexture(RenderTargetsType type) { return *m_renderTargets[type]; }

    u32                             GetMsaaQuality() { return m_4xMsaaQuality; }
    D3D_VIEWPORT&                   GetViewPort(RenderTargetsType type) { return m_ScreenViewPort[type]; }
    D3D_RECT&                       GetScissorRect() { return m_scissorRect; }
    
    std::string                     GetDefaultWindowTitle();
    void                            SetWindowTitle(std::string value);

    bool                            IsSecondaryDrawCall() { return m_secondaryDrawCall; }
    void                            SetSecondaryDrawCall(bool isSecondary) { m_secondaryDrawCall = isSecondary; }

protected:
    virtual void                    CreateDescriptorHeaps() = 0;
    virtual void                    SetupPipeline();
    virtual void                    SetupDevice();
    virtual void                    SetupSwapChain();
    virtual void                    SetupRenderTargets();
    virtual void                    SetupDepthStencilBuffer();
    virtual void                    SetupDepthStencilStates();
    virtual void                    SetupDepthView();
    virtual void                    SetupViewPort();
    virtual void                    SetupShadersAndRenderPool();

    void                            PresentRenderTarget(ID3D12GraphicsCommandList* cmdList) {}

protected:
    ID3DDevice*                     m_pD3DDevice;
    XC_ShaderContainer*             m_XCShaderSystem;

    RenderingPool*                  m_renderingPool;
    SharedDescriptorHeap*           m_sharedDescriptorHeap;
    GPUResourceSystem*              m_gpuResourceSystem;

    RenderableTexture*              m_renderTargets[RENDERTARGET_MAX];
    GPUResource*                    m_depthStencilResource[RENDERTARGET_MAX];

    D3D_VIEWPORT                    m_ScreenViewPort[RENDERTARGET_MAX];
    D3D_RECT                        m_scissorRect;

    XCVec4                          m_clearColor;
    bool                            m_secondaryDrawCall;

    u32                             m_4xMsaaQuality;
    bool                            m_Enable4xMsaa;

    i32                             m_ClientWidth;
    i32                             m_ClientHeight;
    
    HWND                            m_hMainWnd;
    bool                            m_initDone;
};