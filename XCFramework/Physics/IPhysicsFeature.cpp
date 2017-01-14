/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "IPhysicsFeature.h"
#include "IPhysicsBoundVolume.h"

IPhysicsFeature::IPhysicsFeature()
    : m_damping(0.0f)
    , m_inverseMass(0.0f)
    , m_mass(0.0f)
    , m_isDirty(false)
    , m_physicsBoundVolume(nullptr)
{
}

void IPhysicsFeature::Init(const PhysicsDesc& desc)
{
    m_position = desc.m_position;
    m_mass = desc.m_mass;
    m_damping = desc.m_dampForce;

    if(m_mass >= InfiniteMass)
    {
        SetInverseMass(0);
        m_mass = 1;
    }
    else
    {
        m_inverseMass = 1 / m_mass;
    }
}

PhysicsBoundType IPhysicsFeature::GetBoundType() const
{ 
    return m_physicsBoundVolume->GetBoundType();
}

void IPhysicsFeature::SetBoundType(IPhysicsBoundVolume* bound)
{
    m_physicsBoundVolume = bound;
}

IPhysicsFeature::~IPhysicsFeature()
{
    XCDELETE(m_physicsBoundVolume);
}

void IPhysicsFeature::Update(f32 dt)
{
}

bool IPhysicsFeature::HasFiniteMass() const
{
    return m_inverseMass >= 0.0f;
}

f32 IPhysicsFeature::GetMass() const
{
    if (m_inverseMass == 0)
    {
        return 0.0f;
    }
    else
    {
        return 1 / m_inverseMass;
    }
}
