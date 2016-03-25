/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/GraphicUtils.h"

class IAppFramework
{
public:
    IAppFramework();
    virtual ~IAppFramework();
    
    HINSTANCE               GetAppInst();
    HWND                    GetMainWnd();

    //Framework Methods
    virtual int             Init()                      = 0;
    
    virtual void            OnResize()                  = 0;
    virtual void            Run()                       = 0;
    virtual void            Update(float dt)       = 0;
    virtual void            Draw( )                = 0;

    virtual void            Destroy()                   = 0;
    
    void                    EnableFullScreenMode(bool enable);
    bool                    IsDeviceLost();
    

protected:
    //Override these methods, if you need to change working
    virtual void            CalculateFramesPerSecond(float dt){}	//Call from update method
    
    //Customized data members
    std::string             m_MainWndCaption;
    bool                    m_Enable4xMsaa;
    
    //Application,Windows and DIrec3D data members
    HINSTANCE               m_hAppInst;
    HWND                    m_hMainWnd;

    bool                    m_AppPaused;

    int                     m_clientWidth;
    int                     m_clientHeight;
};