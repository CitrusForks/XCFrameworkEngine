/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/ApplicationFramework/IAppFramework.h"

IAppFramework::IAppFramework()
{
    m_MainWndCaption = "XC_Framework";
    m_clientWidth = 800;
    m_clientHeight = 600;
}

IAppFramework::~IAppFramework()
{
}

HINSTANCE IAppFramework::GetAppInst()
{
    return m_hAppInst;
}

HWND IAppFramework::GetMainWnd()
{
    return m_hMainWnd;
}

void IAppFramework::EnableFullScreenMode(bool enable)
{
}

bool IAppFramework::IsDeviceLost()
{
    return false;
}

