/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "BasePrecompiledHeader.h"

#include "XCFloat4.h"

namespace XCMath
{
    const XCFloat4 XCFloat4::XCFloat4ZeroVector = XCVec4(0.0f, 0.0f, 0.0f, 0.0f);
    const XCFloat4 XCFloat4::XCQuaternionIdentity = XCVec4(DirectX::XMQuaternionIdentity());
}