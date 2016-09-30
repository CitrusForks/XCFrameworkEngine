/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/ApplicationFramework/IAppFramework.h"

class AppFrameworkWin32 : public IAppFramework
{
public:
    AppFrameworkWin32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    virtual ~AppFrameworkWin32();

    //Framework Methods
    virtual void            CreateMainWindow();

    virtual i32             Init();
    virtual void            OnResize()       = 0;
    virtual void            Run();
    virtual void            Update(f32 dt)   = 0;
    virtual void            Draw()           = 0;
    virtual void            EnableFullScreenMode(bool enable) {}
    virtual void            Destroy();

    virtual LRESULT         MsgProc(HWND hwnd, u32 msg,WPARAM wParam,LPARAM lParam);
    
    bool                    IsDeviceLost();
    static  void            GLRun();

protected:
    virtual void            InitMainWindow();
    virtual void            CalculateFramesPerSecond(f32 dt) {}	//Call from update method

    bool                    m_appInitialized;

private:
    __int64                 m_cntsPerSec = 0;
    f32                     m_secsPerCnt;
    __int64                 m_prevTimeStamp;
};