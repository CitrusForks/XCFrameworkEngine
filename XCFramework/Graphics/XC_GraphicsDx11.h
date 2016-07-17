/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX11)

#include "Engine/System/ISystem.h"
#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Graphics/RenderingPool.h"
#include "Graphics/XC_Textures/RenderableTexture.h"
#include "Graphics/RenderTargetTypes.h"

class XC_GraphicsDx11 : public XC_Graphics
{
public:
    DECLARE_OBJECT_CREATION(XC_GraphicsDx11)

    XC_GraphicsDx11(void);
    ~XC_GraphicsDx11(void);
    
    void                        InitGraphicsWindow(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    IDXGISwapChain*             GetSwapChain()      { return m_pSwapChain;   }

    ID3DDeviceContext*          GetDeviceContext() { return m_pD3DDeviceContext; }
    ID3DCommandAllocator*       GetDeviceCommandAllocator() { return nullptr; }
    ID3DCommandQueue*           GetCommandQueue() { return nullptr; }
    ID3DResource*               GetCurrentFrameRenderTarget() { return nullptr; }
    CPU_DESCRIPTOR_HANDLE       GetRTVCPUDescHandler() { return 0; }
    CPU_DESCRIPTOR_HANDLE       GetDSVCPUDescHandler() { return 0; }

    void                        SetupPipeline();
    void                        SetupDevice();
    void                        SetupSwapChain();
    void                        SetupRenderTargets();

    void                        SetupDepthStencilBuffer();
    void                        SetupDepthStencilStates();
    void                        SetupDepthView();
    void                        SetupViewPort();

    void                        Destroy();
    void                        Update(f32 dt);
    void                        BeginScene();
    void                        EndScene();
    void                        BeginSecondaryScene();
    void                        EndSecondaryScene();
    void                        GoFullscreen(bool go);
    void                        OnResize(i32 _width, i32 _height);
    void                        TurnOffZ();
    void                        TurnOnZ();
    void                        SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);

    ID3DDepthStencilView&       GetDepthStencilView(RenderTargetsType type) { return type == RENDERTARGET_MAIN_0 ? *m_pDepthStencilView : *m_pDepthStencilViewLiveDrive; }

private:
    IDXGISwapChain*             m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC        m_SwapChainDesc;
    IDXGIFactory*               m_pdxgiFactory;
    D3D_TEXTURE2D_DESC          m_depthBufferDesc;
    D3D_TEXTURE2D_DESC          m_depthBufferDescLiveDrive;

    //Common to both win32 and durango
    ID3DDeviceContext*        m_pD3DDeviceContext;
    ID3DRenderTargetView*     m_pRenderTargetView;
    
    ID3DTexture2D*            m_pDepthStencilBuffer;
    ID3DDepthStencilView*     m_pDepthStencilView;
    ID3DTexture2D*            m_pDepthStencilBufferLiveDrive;
    ID3DDepthStencilView*     m_pDepthStencilViewLiveDrive;

    ID3DDepthStencilState*    m_depthStencilState;
    ID3DDepthStencilState*    m_depthDisabledStencilState;
    ID3DDepthStencilState*    m_depthStencilLessEqualState;
    
    D3D_FEATURE_LEVEL           m_featureLevel;
};
#endif