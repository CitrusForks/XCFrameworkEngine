/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"
#include "InputKeyMapping.h"

class XCInput : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(XCInput)

    XCInput();
    virtual ~XCInput();

    virtual void                Init(HINSTANCE hAppInstance, HWND hAppWnd, u64 keyboardCoopFlags, u64 mouseCoopFlags);
    virtual void                Destroy();

    virtual void                Update();
    virtual bool                KeyDown(char key);
    virtual bool                MouseButtonDown(i32 button);
    virtual f32               MouseDX();
    virtual f32               MouseDY();
    virtual f32               MouseDZ();

    //Fake Inputs
    virtual void                InitFakeInputs();
    virtual void                SetFakeKeyDown(char key);
    virtual void                SetFakeMouseInput(FakeMouseInputType type, f32 value);
    virtual void                ResetAllFakeInputs();
};