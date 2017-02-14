/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "RigidBody.h"
#include "PhysicsPlayground.h"
#include "IPhysicsBoundVolume.h"

void RigidBody::Init(const PhysicsDesc& desc)
{
    IPhysicsFeature::Init(desc);

    m_ForceAccumulator = XCVec3(0, 0, 0);
    m_ContactNormal = XCVec3(0, 0, 0);
}

RigidBody::~RigidBody()
{
}

void RigidBody::AddForce(const XCVec4& _newForce)
{
    m_isDirty = true;
    m_ForceAccumulator += _newForce;
}

void RigidBody::AddRotationalForce(const XCVec4& newForce)
{
    m_isDirty = true;
    m_orientation = newForce;
}

void RigidBody::Update(f32 dt)
{
    PhysicsPlayground& playground = SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");

    //Add gravity to force accumulator
    if(playground.IsGravityEnabled())
    {
        m_ForceAccumulator += playground.GetAcceleratedGravity();
    }

    XCVec4 currentAcceleration = m_Acceleration;

    //Update the position
    m_position += (m_velocity * dt);

    //Update the Resulting Acceleration
    currentAcceleration += m_ForceAccumulator * m_inverseMass;

    //Update linear Velocity
    m_velocity += currentAcceleration * dt;

    //Impose Draging force
    m_velocity *= (f32)pow(m_damping, dt);

    ClearForce();

    //Update the bound volume
    m_physicsBoundVolume->Transform(m_position, m_orientation);
}

void RigidBody::ClearForce()
{
    m_ForceAccumulator = XCVec3(0, 0, 0);
}

void RigidBody::ClearVelocity()
{
    m_velocity = XCVec3(0, 0, 0);
}