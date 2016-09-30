/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(WIN_32)

#include "Engine/Input/XCInput.h"

class XCInputWin32 : public XCInput
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(XCInputWin32)

    XCInputWin32();
    virtual ~XCInputWin32();

    void                        Init(HINSTANCE hAppInstance, HWND hAppWnd, u64 keyboardCoopFlags, u64 mouseCoopFlags);
    void                        Destroy();

    void                        Update();
    bool                        KeyDown(char key);
    bool                        MouseButtonDown(i32 button);
    f32                       MouseDX();
    f32                       MouseDY();
    f32                       MouseDZ();

    //Fake Inputs
    void                        InitFakeInputs();
    void                        SetFakeKeyDown(char key);
    void                        SetFakeMouseInput(FakeMouseInputType type, f32 value);
    void                        ResetAllFakeInputs();

private:

    char                        m_KeyboardState[256];
    std::map<char, bool>        m_fakeKeyboardInputsMap;
    std::map<i32, f32>        m_fakeMouseInputsMap;

    IDirectInput8*              m_pDInput;
    IDirectInputDevice8*        m_pKeyboard;
    IDirectInputDevice8*        m_pMouse;
    DIMOUSESTATE2               m_MouseState;
};

#endif