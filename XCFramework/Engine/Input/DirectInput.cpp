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

void DirectInput::Init(HINSTANCE _hAppInstance, HWND _hAppWnd, unsigned long keyboardCoopFlags,unsigned long mouseCoopFlags)
{
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

void DirectInput::SetFakeMouseInput(FakeMouseInputType type, float value)
{
}

bool DirectInput::KeyDown(char key)
{
    return false;
}

bool DirectInput::MouseButtonDown(int button)
{
    return false;
}

float DirectInput::MouseDX()
{
    return (float)0.0f;
}

float DirectInput::MouseDY()
{
    return (float)0.0f;
}

float DirectInput::MouseDZ()
{
    return (float)0.0f;
}
