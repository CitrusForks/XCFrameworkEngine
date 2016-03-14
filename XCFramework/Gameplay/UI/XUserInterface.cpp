/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "Gameplay/UI/XUserInterface.h"
#include "Engine/Utils/GraphicUtils.h"

XUserInterface* XUserInterface::ms_pXUserInterface = nullptr;

/*
extern D3DDevice				*g_pd3dDevice;
extern D3DPRESENT_PARAMETERS	g_d3dpp;
extern TimeInfo					g_Time;
*/

XUserInterface::~XUserInterface(void)
{
	Destroy();
}

void XUserInterface::Destroy()
{
	ReleaseCOM(m_pIRedBrush);
	ReleaseCOM(m_pITextFormat);
	ReleaseCOM(m_pIRenderTarget);
	/*
	for(unsigned int i=0; i< m_hFonts.size(); i++)
	if (m_hFonts[i] != 0)
	{
#ifdef XBOX
		XuiReleaseFont(m_hFonts[i]);
#else
		m_hFonts[i]->Release();
#endif

	}

#ifdef XBOX
	if(m_hDC != 0 )
		XuiRenderDestroyDC(m_hDC);

	XuiRenderUninit();

	XuiUninit();
#endif
	*/
}


XUserInterface*	XUserInterface::GetInstance()
{
	if(ms_pXUserInterface == nullptr)
		ms_pXUserInterface = new XUserInterface();

	return ms_pXUserInterface;
}


void XUserInterface::InitXui(HWND hWnd)
{
#ifdef XBOX
	XUIInitParams initParams = { 0 };

	XUI_INIT_PARAMS( initParams );

	TypefaceDescriptor desc = { 0 };
	desc.szTypeface = L"Arial Unicode MS";
	desc.szLocator  = L"file://Game:/Content/Fonts/xarialuni.ttf";

	XuiRenderInitShared(g_pd3dDevice, &g_d3dpp, XuiD3DXTextureLoader);

	XuiRenderCreateDC(&m_hDC);

	XuiInit( &initParams );

	XuiRegisterTypeface( &desc, TRUE );
#elif defined(WIN32)
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
#endif
	//Set Animation Variables
	/*m_startTime	  = g_Time.fAppTime;
	m_alphaFactor = 255 * m_startTime;*/
}


void XUserInterface::XCreateFont(float fontSize)
{
	/*HXUIFONT font;
#ifdef XBOX
	XuiCreateFont( L"Arial Unicode MS", fontSize, XUI_FONT_STYLE_NORMAL, 0, &font);
#elif defined(WIN32)
	D3DXFONT_DESC fontDesc;

	fontDesc.Height = fontSize * 2;
	fontDesc.Width = 0;
	fontDesc.Weight = 0;
	fontDesc.MipLevels = 1;
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(fontDesc.FaceName, "Arial Unicode MS");

	HRESULT hr = D3DXCreateFontIndirect(g_pd3dDevice, &fontDesc, &font);
#endif
	m_hFonts.push_back(font);*/
}


void XUserInterface::Update()
{
	/*float dt = g_Time.fAppTime - m_startTime;
	m_alphaFactor = 255 *dt;

	if(m_alphaFactor >= 255)
	{
		m_startTime = g_Time.fAppTime;
		m_alphaFactor = 1;
	}*/
	XDrawText();
}

void XUserInterface::UIRenderBeginScene()
{
#ifdef XBOX
	XuiRenderBegin(m_hDC, D3DCOLOR_ARGB( 255, 0, 0, 100));

	D3DXMATRIX	matView;
	D3DXMatrixIdentity(&matView);
	XuiRenderSetViewTransform(m_hDC, &matView);
#endif
}

void XUserInterface::XDrawText(int _fontTypeIndex, XCVec4 color, float x, float y, LPCWSTR text)
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
	/*
	D3DCOLOR d3dColor = D3DCOLOR_ARGB((unsigned int)color.w, (unsigned int)color.x, (unsigned int)color.y, (unsigned int)color.z);

#ifdef XBOX
	XUIRect clipRect( 0, 0, SCREEN_WIDTH - x, SCREEN_HEIGHT - y );
	XuiMeasureText( m_hFonts[_fontTypeIndex], text, -1, XUI_FONT_STYLE_NORMAL, 0, &clipRect );

	// Set the text position in the device context
	D3DXMATRIX matXForm;
	D3DXMatrixIdentity( &matXForm );
	matXForm._41 = x;
	matXForm._42 = y;
	XuiRenderSetTransform( m_hDC, &matXForm );

	// Select the font and color into the device context
	XuiSelectFont( m_hDC, m_hFonts[_fontTypeIndex] );
	XuiSetColorFactor( m_hDC, ( DWORD )d3dColor );
	// Draw the text
	XuiDrawText( m_hDC, text, XUI_FONT_STYLE_LEFT_ALIGN, 0, &clipRect );
#elif defined(WIN32)
	RECT clipRect = { x, y, 0, 0 };
	m_hFonts[_fontTypeIndex]->DrawTextW(0, text, -1, &clipRect, DT_NOCLIP, d3dColor);
#endif*/
}

void XUserInterface::DrawAnimatedText(int _fontTypeIndex, XCVec4 color, float x, float y, LPCWSTR text, float animSpeed)
{
	m_alphaFactor *= animSpeed;
	color.w		   = m_alphaFactor;

	XDrawText(_fontTypeIndex, color, x, y, text);
}

void XUserInterface::UIRenderEndScene()
{
#ifdef XBOX
	XuiRenderEnd(m_hDC);
	XuiRenderPresent(m_hDC, NULL, NULL, NULL);
#endif
}
