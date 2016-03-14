/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(WIN_32)
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

enum FakeMouseInputType
{
    MOUSEINPUT_DX = 0,
    MOUSEINPUT_DY = 1
};

#if defined(XC_ORBIS)

#include "InputKeyMappingOrbis.h"

#elif defined(WIN32)

enum KeyInputTypes
{
    INPUT_UP = DIK_W,
    INPUT_DOWN = DIK_S,
    INPUT_LEFT = DIK_A,
    INPUT_RIGHT = DIK_D,
    INPUT_KEY_I = DIK_I,
    INPUT_KEY_K = DIK_K,
    INPUT_KEY_SPACE = DIK_SPACE,

    INPUT_KEY_1 = DIK_1,
    INPUT_KEY_2 = DIK_2,
    INPUT_KEY_3 = DIK_3,
    INPUT_KEY_4 = DIK_4,
    INPUT_KEY_5 = DIK_5,
    INPUT_KEY_6 = DIK_6,
    INPUT_KEY_ESCAPE = DIK_ESCAPE
};

enum MouseKeyInputTypes
{
    MOUSE_INPUT_BUTTON_L = 0,
    MOUSE_INPUT_BUTTON_R = 1
};

#endif
