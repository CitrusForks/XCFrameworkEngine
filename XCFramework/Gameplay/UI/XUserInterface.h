/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#ifdef XBOX
#include<xui.h>
#elif defined(WIN32)
//#define HXUIFONT ID3DXFont*
#include <D2d1.h>
#include <dwrite.h>
#endif

class XUserInterface
{
private:
    static XUserInterface				*ms_pXUserInterface;
#ifdef XBOX
    HXUIDC								m_hDC;
#endif
    //std::vector<HXUIFONT>				m_hFonts;
    float								m_alphaFactor;
    float								m_startTime;

#if defined(WIN32)
    ID2D1HwndRenderTarget*				m_pIRenderTarget;
    IDWriteTextFormat*					m_pITextFormat;
    IDWriteFactory*						m_pIDWriteFactory;
    ID2D1SolidColorBrush*				m_pIRedBrush;
    ID2D1Factory*						m_pDirect2dFactory;

    HWND								m_hWnd;
#endif

    XUserInterface()
    {

    }

public:
    ~XUserInterface(void);

    static XUserInterface*				GetInstance();
    void								InitXui(HWND hWnd);
    void								XCreateFont(float fontSize);
    void								Destroy();
    void								Update();
    void								UIRenderBeginScene();
    void								UIRenderEndScene();
    void								XDrawText();
    void								XDrawText(int _fontTypeIndex, XCVec4 _color, float _x, float _y, LPCWSTR _text);
    void								DrawAnimatedText(int _fontTypeIndex, XCVec4 _color, float _x, float _y, LPCWSTR _text, float _animSpeed);
};

#define XUSERINTERFACE					XUserInterface::GetInstance()