/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"
#include "XCPhysics/XPhysics.h"

bool XPhysics::s_enableGravity = false;

XPhysics::XPhysics()
{
    XCVec3 direction = XCVec3(0, -5, 0);
    m_GAcceleration = XMLoadFloat3(&direction);
}

void XPhysics::InitXPhysics(XCVecIntrinsic4 _position, XCVecIntrinsic4 _velocity, XCVecIntrinsic4 _acceleration, float _mass, float _damping)
{
    m_Position = _position;
    m_Velocity = _velocity;
    m_Acceleration = _acceleration;
    m_Mass = _mass;
    m_Damping = _damping;

    if(_mass >= 999)
    {
        SetInverseMass(0);
        m_Mass = 1;
    }
    else
    {
        m_InverseMass = 1 / m_Mass;
    }

    m_ForceAccumulator   = Float3ToVec(XCVec3(0, 0, 0));
    m_ContactNormal      = Float3ToVec(XCVec3(0, 0, 0));
}

XPhysics::~XPhysics()
{
}

void XPhysics::AddForce(XCVecIntrinsic4 _newForce)
{
    m_ForceAccumulator += _newForce;
}

void XPhysics::Integrator(float dt)
{
    //Add gravity to force accumulator
    if (s_enableGravity)
        m_ForceAccumulator += m_GAcceleration;

    XCVecIntrinsic4 currentAcceleration = m_Acceleration;

    //Update the position
    m_Position += (m_Velocity * dt);

    //Update the Resulting Acceleration
    currentAcceleration += m_ForceAccumulator * m_InverseMass;

    //Update linear Velocity
    m_Velocity += currentAcceleration * dt;

    //Impose Draging force
    m_Velocity *= (float)pow( m_Damping, dt);
}

void XPhysics::ClearForce()
{
    XCVec3 vec = XCVec3(0, 0, 0);
    m_ForceAccumulator = XMLoadFloat3(&vec);
}

void XPhysics::ClearVelocity()
{
    XCVec3 vec = XCVec3(0, 0, 0);
    m_Velocity = XMLoadFloat3(&vec);
}

bool XPhysics::HasFiniteMass()
{
    return m_InverseMass >= 0.0f;
}

float XPhysics::GetMass()
{
    if (m_InverseMass == 0)
    {
        return 0.0f;
    }
    else
    {
        return 1 / m_InverseMass;
    }
}
