/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(WIN_32)

#include "Engine/Input/Directinput.h"

class DirectInput_Win32 : public DirectInput
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(DirectInput_Win32)

    DirectInput_Win32();
    ~DirectInput_Win32();

    void                        Init(HINSTANCE hAppInstance, HWND hAppWnd, unsigned long keyboardCoopFlags, unsigned long mouseCoopFlags);
    void                        Destroy();

    void                        Update();
    bool                        KeyDown(char key);
    bool                        MouseButtonDown(int button);
    float                       MouseDX();
    float                       MouseDY();
    float                       MouseDZ();

    //Fake Inputs
    void                        InitFakeInputs();
    void                        SetFakeKeyDown(char key);
    void                        SetFakeMouseInput(FakeMouseInputType type, float value);
    void                        ResetAllFakeInputs();
private:

    char                        m_KeyboardState[256];
    std::map<char, bool>        m_fakeKeyboardInputsMap;
    std::map<int, float>        m_fakeMouseInputsMap;

    IDirectInput8*              m_pDInput;
    IDirectInputDevice8*        m_pKeyboard;
    IDirectInputDevice8*        m_pMouse;
    DIMOUSESTATE2               m_MouseState;
};

#endif