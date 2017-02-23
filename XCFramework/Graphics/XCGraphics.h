/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"
#include "Base/Thread/CriticalSectionRecursive.h"

#include "Graphics/XCShaders/XCShaderContainer.h"
#include "Graphics/RenderingPool.h"
#include "Graphics/XCTextures/RenderableTexture.h"
#include "Graphics/RenderTargetTypes.h"

class GPUResourceSystem;
class SharedDescriptorHeap;

class XCGraphics : public ISystem
{
public:
    static const u32                WindowWidth = 1024;
    static const u32                WindowHeight = 768;

    XCGraphics(void);
    virtual ~XCGraphics(void);
    
    virtual void                    Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    virtual void                    BeginScene();
    virtual void                    Render();
    virtual void                    EndScene();
    virtual void                    BeginSecondaryScene();
    virtual void                    EndSecondaryScene();
    virtual void                    Update(f32 dt);
    virtual void                    Destroy();

    virtual ID3DDeviceContext*      GetDeviceContext() = 0;
    virtual GPUResource*            GetDepthStencilView(RenderTargetsType type) { return m_depthStencilResource[type]; };
    virtual ID3DPipelineState*      GetPipelineState() { return nullptr; }

    virtual void                    GoFullscreen(bool go);
    virtual void                    OnResize(i32 _width, i32 _height);

    virtual void                    TurnOffZ();
    virtual void                    TurnOnZ();
    virtual void                    SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);
    virtual void                    SetRenderableTargets(ID3DDeviceContext& context, const std::vector<RenderTargetsType>& types) = 0;
    virtual void                    ClearRTVAndDSVs(ID3DDeviceContext& context, std::vector<RenderTargetsType>& type, XCVec4& clearColor) = 0;

    u32                             GetCurrentRTVFrameIndex() const { return m_frameIndex; }
    XCShaderContainer&              GetShaderContainer() { return *m_xcShaderSystem; }
    RenderingPool&                  GetRenderingPool() { return *m_renderingPool; }
    RenderableTexture&              GetRenderTexture(RenderTargetsType type) { return *m_renderTargets[type]; }

    u32                             GetMsaaQuality() { return m_4xMsaaQuality; }
    D3D_VIEWPORT&                   GetViewPort(RenderTargetsType type) { return m_ScreenViewPort[type]; }
    D3D_RECT&                       GetScissorRect() { return m_scissorRect; }
    
    std::string                     GetDefaultWindowTitle();
    void                            SetWindowTitle(std::string value);

    bool                            IsSecondaryDrawCall() { return m_secondaryDrawCall; }
    void                            SetSecondaryDrawCall(bool isSecondary) { m_secondaryDrawCall = isSecondary; }

    ID3DDevice*                     GetDevice();

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

    void                            OnRenderComplete();
    void                            PresentRenderTarget(ID3D12GraphicsCommandList* cmdList) {}

protected:
    ID3DDevice*                     m_pD3DDevice;
    ID3DDeviceContext*              m_pD3DDeviceContext;

    XCShaderContainer*              m_xcShaderSystem;

    RenderingPool*                  m_renderingPool;
    SharedDescriptorHeap*           m_sharedDescriptorHeap;
    GPUResourceSystem*              m_gpuResourceSystem;

    RenderableTexture*              m_renderTargets[RenderTargetType_Max];
    GPUResource*                    m_depthStencilResource[RenderTargetType_Max];

    D3D_VIEWPORT                    m_ScreenViewPort[RenderTargetType_Max];
    D3D_RECT                        m_scissorRect;

    bool                            m_secondaryDrawCall;

    //FrameIndex
    u32                             m_frameIndex;

    u32                             m_4xMsaaQuality;
    bool                            m_enable4xMsaa;

    i32                             m_clientWidth;
    i32                             m_clientHeight;
    
    HWND                            m_hMainWnd;
    bool                            m_initDone;
};