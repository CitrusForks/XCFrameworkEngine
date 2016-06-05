/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gameplay/GameActors/TPCChaseableActor.h"


TPCChaseableActor::TPCChaseableActor(void)
{
    m_distanceFromTarget = XMVectorZero();
}

TPCChaseableActor::~TPCChaseableActor(void)
{
}

void TPCChaseableActor::SetChasingAxis(XCVecIntrinsic4& look, XCVecIntrinsic4& right, XCVecIntrinsic4& up)
{
    m_look = &look;
    m_right = &right;
    m_up = &up;
}
