/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "TPCChaseableActor.h"


TPCChaseableActor::TPCChaseableActor(void)
{
    m_distanceFromTarget = XCFloat4::XCFloat4ZeroVector;
}

TPCChaseableActor::~TPCChaseableActor(void)
{
}

void TPCChaseableActor::SetChasingAxis(XCVec4& look, XCVec4& right, XCVec4& up)
{
    m_look = &look;
    m_right = &right;
    m_up = &up;
}
