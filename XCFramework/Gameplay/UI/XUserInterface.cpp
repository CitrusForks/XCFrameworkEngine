/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"
#include "Gameplay/UI/XUserInterface.h"
#include "Engine/Utils/EngineUtils.h"

XUserInterface* XUserInterface::ms_pXUserInterface = nullptr;

XUserInterface::~XUserInterface(void)
{
    Destroy();
}

void XUserInterface::Destroy()
{
    ReleaseCOM(m_pIRedBrush);
    ReleaseCOM(m_pITextFormat);
    ReleaseCOM(m_pIRenderTarget);
}

XUserInterface*	XUserInterface::GetInstance()
{
    if(ms_pXUserInterface == nullptr)
        ms_pXUserInterface = new XUserInterface();

    return ms_pXUserInterface;
}

void XUserInterface::InitXui(HWND hWnd)
{
    HRESULT hr = S_OK;

    m_hWnd = hWnd;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    RECT rc;
    GetClientRect(m_hWnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top
        );

    // Create a Direct2D render target.
    hr = m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        D2D1::HwndRenderTargetProperties(m_hWnd, size),
        &m_pIRenderTarget
        );

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**> (&m_pIDWriteFactory));

    if (SUCCEEDED(hr))
    {
        hr = m_pIDWriteFactory->CreateTextFormat(
            L"Arial",
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            10.0f * 96.0f / 32.0f,
            L"en-US",
            &m_pITextFormat
            );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pIRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Red),
            &m_pIRedBrush
            );
    }

    XDrawText();
}


void XUserInterface::XCreateFont(f32 fontSize)
{
}

void XUserInterface::Update()
{
    XDrawText();
}

void XUserInterface::UIRenderBeginScene()
{
}

void XUserInterface::XDrawText(i32 _fontTypeIndex, XCVec4& color, f32 x, f32 y, LPCWSTR text)
{
}

void XUserInterface::XDrawText()
{
    D2D1_RECT_F layoutRect = D2D1::RectF(0.f, 0.f, 1000.f, 500.f);

    // Actually draw the text at the origin.
    m_pIRenderTarget->BeginDraw();
    D2D1_COLOR_F color = { 1, 1, 1, 1 };
    m_pIRenderTarget->Clear(color);
        m_pIRenderTarget->DrawText(
            L"Game Engine - XC Framework \n Loading... Please Wait!",
            wcslen(L"Game Engine - XC Framework \n Loading... Please Wait!"),
            m_pITextFormat,
            layoutRect,
            m_pIRedBrush
            );
        m_pIRenderTarget->EndDraw();
}

void XUserInterface::DrawAnimatedText(i32 _fontTypeIndex, XCVec4& color, f32 x, f32 y, LPCWSTR text, f32 animSpeed)
{
    m_alphaFactor *= animSpeed;
    color.Set<W>(m_alphaFactor);

    XDrawText(_fontTypeIndex, color, x, y, text);
}

void XUserInterface::UIRenderEndScene()
{
}
