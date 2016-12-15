/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "IPhysicsFeature.h"

IPhysicsFeature::IPhysicsFeature()
{
}

void IPhysicsFeature::InitXPhysics(const XCVec4& position, const XCVec4& velocity, const XCVec4& acceleration, f32 mass, f32 damping)
{
    m_Position = position;
    m_Velocity = velocity;
    m_Acceleration = acceleration;
    m_Mass = mass;
    m_Damping = damping;

    if (mass >= 999)
    {
        SetInverseMass(0);
        m_Mass = 1;
    }
    else
    {
        m_InverseMass = 1 / m_Mass;
    }

    m_ForceAccumulator = XCVec3(0, 0, 0);
    m_ContactNormal = XCVec3(0, 0, 0);
}

IPhysicsFeature::~IPhysicsFeature()
{
}

void IPhysicsFeature::AddForce(const XCVec4& _newForce)
{
    m_ForceAccumulator += _newForce;
}

void IPhysicsFeature::Integrator(f32 dt)
{
    //Add gravity to force accumulator
    m_ForceAccumulator += PhysicsPlayground::GetAcceleratedGravity();

    XCVec4 currentAcceleration = m_Acceleration;

    //Update the position
    m_Position += (m_Velocity * dt);

    //Update the Resulting Acceleration
    currentAcceleration += m_ForceAccumulator * m_InverseMass;

    //Update linear Velocity
    m_Velocity += currentAcceleration * dt;

    //Impose Draging force
    m_Velocity *= (f32)pow(m_Damping, dt);
}

void IPhysicsFeature::ClearForce()
{
    m_ForceAccumulator = XCVec3(0, 0, 0);
}

void IPhysicsFeature::ClearVelocity()
{
    m_Velocity = XCVec3(0, 0, 0);
}

bool IPhysicsFeature::HasFiniteMass() const
{
    return m_InverseMass >= 0.0f;
}

f32 IPhysicsFeature::GetMass() const
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
