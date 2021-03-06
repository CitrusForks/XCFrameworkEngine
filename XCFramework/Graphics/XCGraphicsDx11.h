/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(XCGRAPHICS_DX11)

#include "Base/System/ISystem.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/RenderTargetTypes.h"
#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

class XCGraphicsDx11 : public XCGraphics
{
public:
    DECLARE_OBJECT_CREATION(XCGraphicsDx11)

    XCGraphicsDx11(void);
    ~XCGraphicsDx11(void);
    
    void                        Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa);
    void                        BeginScene();
    void                        EndScene();
    void                        BeginSecondaryScene();
    void                        EndSecondaryScene();
    void                        Update(f32 dt);
    void                        Destroy();

    ID3DDeviceContext*          GetDeviceContext() { return m_pD3DDeviceContext; }
    void                        GoFullscreen(bool go);
    void                        OnResize(i32 _width, i32 _height);
    void                        TurnOffZ();
    void                        TurnOnZ();
    void                        SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn);

    void                        SetRenderableTargets(ID3DDeviceContext& context, const std::vector<RenderTargetsType>& types) override;
    void                        ClearRTVAndDSVs(ID3DDeviceContext& context, std::vector<RenderTargetsType>& type, XCVec4& clearColor) override;

protected:
    void                        CreateDescriptorHeaps();
    void                        CreateGPUResourceSystem();

    void                        SetupPipeline();
    void                        SetupDevice();
    void                        SetupSwapChain();
    void                        SetupRenderTargets();
    void                        SetupRenderQuad();

    void                        SetupDepthStencilStates();
    void                        SetupDepthView();
    void                        SetupViewPort();

private:
    ID3DSwapChain*              m_pSwapChain;
    DXGI_SWAP_CHAIN_DESC        m_SwapChainDesc;
    IDXGIFactory*               m_pdxgiFactory;

    ID3DRenderTargetView*       m_pRenderTargetView;
    VertexBuffer<VertexPosTex>* m_renderQuadVB;
    IndexBuffer<u32>*           m_renderQuadIB;

    ID3DDepthStencilState*      m_depthStencilState;
    ID3DDepthStencilState*      m_depthDisabledStencilState;
    ID3DDepthStencilState*      m_depthStencilLessEqualState;
    
    D3D_FEATURE_LEVEL           m_featureLevel;
};
#endif