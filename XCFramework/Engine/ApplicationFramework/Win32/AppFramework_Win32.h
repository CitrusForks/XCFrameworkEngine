/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/ApplicationFramework/IAppFramework.h"

class AppFramework_Win32 : public IAppFramework
{
public:
    AppFramework_Win32(HINSTANCE hInstance, std::string winCaption, bool enable4xMsaa);
    virtual ~AppFramework_Win32();

    //Framework Methods
    virtual void            CreateMainWindow();

    virtual int             Init();
    virtual void            OnResize()       = 0;
    virtual void            Run();
    virtual void            Update(float dt) = 0;
    virtual void            Draw()           = 0;
    virtual void            EnableFullScreenMode(bool enable) {}
    virtual void            Destroy();

    virtual LRESULT         MsgProc(HWND hwnd, unsigned int msg,WPARAM wParam,LPARAM lParam);
    
    bool                    IsDeviceLost();
    static  void            GLRun();

protected:
    //Override these methods please if you need to change working
    virtual void            InitMainWindow();
    virtual void            CalculateFramesPerSecond(float dt) {}	//Call from update method

    bool                    m_appInitialized;
private:
    __int64                 m_cntsPerSec = 0;
    float                   m_secsPerCnt;
    __int64                 m_prevTimeStamp;
};