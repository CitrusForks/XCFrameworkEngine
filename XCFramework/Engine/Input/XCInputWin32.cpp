/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/Input/XCInputWin32.h"

#if defined(WIN_32)

XCInputWin32::XCInputWin32()
{
    ZeroMemory(m_KeyboardState, sizeof(m_KeyboardState));
    ZeroMemory(&m_MouseState, sizeof(m_MouseState));
    m_pDInput = nullptr;
    m_pKeyboard = nullptr;
    m_pMouse = nullptr;
}

void XCInputWin32::Init(HINSTANCE _hAppInstance, HWND _hAppWnd, u64 keyboardCoopFlags,u64 mouseCoopFlags)
{
#if !defined(EDITOR)
    ValidateResult(DirectInput8Create(  _hAppInstance,
                            DIRECTINPUT_VERSION,
                            IID_IDirectInput8,
                            (void**)&m_pDInput, 0));

    ValidateResult(m_pDInput->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,0));
    m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
    m_pKeyboard->SetCooperativeLevel(_hAppWnd,keyboardCoopFlags);
    m_pKeyboard->Acquire();

    ValidateResult(m_pDInput->CreateDevice(GUID_SysMouse,&m_pMouse,0));
    m_pMouse->SetDataFormat(&c_dfDIMouse2);
    m_pMouse->SetCooperativeLevel(_hAppWnd,mouseCoopFlags);
    m_pMouse->Acquire();
#endif
}

void XCInputWin32::Destroy()
{
    ReleaseCOM(m_pDInput);
    m_pKeyboard->Unacquire();
    m_pMouse->Unacquire();
    ReleaseCOM(m_pKeyboard);
    ReleaseCOM(m_pMouse);
}

XCInputWin32::~XCInputWin32()
{
}

void XCInputWin32::InitFakeInputs()
{
    m_fakeKeyboardInputsMap[INPUT_UP] = 0;
    m_fakeKeyboardInputsMap[INPUT_LEFT] = 0;
    m_fakeKeyboardInputsMap[INPUT_DOWN] = 0;
    m_fakeKeyboardInputsMap[INPUT_RIGHT] = 0;

    m_fakeKeyboardInputsMap[DIK_Z] = 0;
    m_fakeKeyboardInputsMap[DIK_X] = 0;
    

    //Mouse Inputs
    m_fakeMouseInputsMap[MOUSEINPUT_DX] = 0.0f;
    m_fakeMouseInputsMap[MOUSEINPUT_DY] = 0.0f;
}

void XCInputWin32::ResetAllFakeInputs()
{
    InitFakeInputs();
}

void XCInputWin32::Update()
{
    HRESULT hr;

    ResetAllFakeInputs();

    //Poll Keyboard
    if (m_pKeyboard != nullptr)
    {
        hr = m_pKeyboard->GetDeviceState(sizeof(m_KeyboardState), (void**)&m_KeyboardState);

        if (FAILED(hr))
        {
            ZeroMemory(m_KeyboardState, sizeof(m_KeyboardState));
            hr = m_pKeyboard->Acquire();
        }
    }

    //Poll mouse
    if (m_pMouse)
    {
        hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_MouseState);
        if (FAILED(hr))
        {
            ZeroMemory(&m_MouseState, sizeof(m_MouseState));
            hr = m_pMouse->Acquire();
        }
    }
}

void XCInputWin32::SetFakeKeyDown(char key)
{
    auto it = m_fakeKeyboardInputsMap.find(key);
    if (it != m_fakeKeyboardInputsMap.end())
    {
        it->second = true;
    }
}

void XCInputWin32::SetFakeMouseInput(FakeMouseInputType type, f32 value)
{
    auto it = m_fakeMouseInputsMap.find(type);
    if (it != m_fakeMouseInputsMap.end())
    {
        it->second = value;
    }
}

bool XCInputWin32::KeyDown(char key)
{
    auto it = m_fakeKeyboardInputsMap.find(key);
    if (it != m_fakeKeyboardInputsMap.end() && it->second)
    {
        return true;
    }

    return (m_KeyboardState[key]&0x80) != 0;
}

bool XCInputWin32::MouseButtonDown(i32 button)
{
    if (m_MouseState.rgbButtons[0])
    {
        return true;
    }
    return (m_MouseState.rgbButtons[button]&0x80)!=0;
}

f32 XCInputWin32::MouseDX()
{
    auto it = m_fakeMouseInputsMap.find(MOUSEINPUT_DX);
    if (it != m_fakeMouseInputsMap.end() && it->second != 0.0f)
    {
        return it->second;
    }

    return (f32)m_MouseState.lX;
}

f32 XCInputWin32::MouseDY()
{
    auto it = m_fakeMouseInputsMap.find(MOUSEINPUT_DY);
    if (it != m_fakeMouseInputsMap.end() && it->second != 0.0f)
    {
        return it->second;
    }

    return (f32)m_MouseState.lY;
}

f32 XCInputWin32::MouseDZ()
{
    return (f32)m_MouseState.lZ;
}

#endif