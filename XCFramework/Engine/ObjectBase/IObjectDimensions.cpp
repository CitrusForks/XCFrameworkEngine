/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "IObjectDimensions.h"

IObjectDimensions::IObjectDimensions()
{
    m_World               = XMMatrixIdentity();
    m_MScaling            = m_World;
    m_MRotation           = m_World;
    m_MTranslation        = m_World;
    m_MInitialRotation     = m_World;
    m_transformedRotation = m_World;

    m_currentPosition = XMVectorZero();
    m_initialRotation = XMVectorZero();
    m_initialScaling  = XMVectorZero();
    m_up              = XMVectorZero();
    m_look            = XMVectorZero();
    m_right           = XMVectorZero();
}

void IObjectDimensions::ApplyRotation(XCVecIntrinsic4 axis)
{
    m_MInitialRotation *= XMMatrixRotationRollPitchYawFromVector(axis);
}