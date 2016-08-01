/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/Input/Directinput.h"

DirectInput::DirectInput()
{
}

void DirectInput::Init(HINSTANCE _hAppInstance, HWND _hAppWnd, u64 keyboardCoopFlags,u64 mouseCoopFlags)
{
    ISystem::Init();
}

void DirectInput::Destroy()
{
}

DirectInput::~DirectInput()
{
}

void DirectInput::InitFakeInputs()
{
}

void DirectInput::ResetAllFakeInputs()
{
}

void DirectInput::Update()
{
}

void DirectInput::SetFakeKeyDown(char key)
{
}

void DirectInput::SetFakeMouseInput(FakeMouseInputType type, f32 value)
{
}

bool DirectInput::KeyDown(char key)
{
    return false;
}

bool DirectInput::MouseButtonDown(i32 button)
{
    return false;
}

f32 DirectInput::MouseDX()
{
    return (f32)0.0f;
}

f32 DirectInput::MouseDY()
{
    return (f32)0.0f;
}

f32 DirectInput::MouseDZ()
{
    return (f32)0.0f;
}
