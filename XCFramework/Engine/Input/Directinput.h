/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/System/ISystem.h"
#include "InputKeyMapping.h"

class DirectInput : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(DirectInput)

    DirectInput();
    virtual ~DirectInput();

    virtual void                Init(HINSTANCE hAppInstance, HWND hAppWnd, unsigned long keyboardCoopFlags, unsigned long mouseCoopFlags);
    virtual void                Destroy();

    virtual void                Update();
    virtual bool                KeyDown(char key);
    virtual bool                MouseButtonDown(int button);
    virtual float               MouseDX();
    virtual float               MouseDY();
    virtual float               MouseDZ();

    //Fake Inputs
    virtual void                InitFakeInputs();
    virtual void                SetFakeKeyDown(char key);
    virtual void                SetFakeMouseInput(FakeMouseInputType type, float value);
    virtual void                ResetAllFakeInputs();
};