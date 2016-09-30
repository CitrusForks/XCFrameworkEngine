/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/Input/XCInput.h"

XCInput::XCInput()
{
}

void XCInput::Init(HINSTANCE _hAppInstance, HWND _hAppWnd, u64 keyboardCoopFlags,u64 mouseCoopFlags)
{
    ISystem::Init();
}

void XCInput::Destroy()
{
}

XCInput::~XCInput()
{
}

void XCInput::InitFakeInputs()
{
}

void XCInput::ResetAllFakeInputs()
{
}

void XCInput::Update()
{
}

void XCInput::SetFakeKeyDown(char key)
{
}

void XCInput::SetFakeMouseInput(FakeMouseInputType type, f32 value)
{
}

bool XCInput::KeyDown(char key)
{
    return false;
}

bool XCInput::MouseButtonDown(i32 button)
{
    return false;
}

f32 XCInput::MouseDX()
{
    return (f32)0.0f;
}

f32 XCInput::MouseDY()
{
    return (f32)0.0f;
}

f32 XCInput::MouseDZ()
{
    return (f32)0.0f;
}
