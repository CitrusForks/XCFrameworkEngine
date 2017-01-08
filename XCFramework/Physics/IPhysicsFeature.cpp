/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "IPhysicsFeature.h"

IPhysicsFeature::IPhysicsFeature()
    : m_damping(0.0f)
    , m_inverseMass(0.0f)
    , m_mass(0.0f)
{
}

void IPhysicsFeature::Init(const PhysicsDesc& desc)
{
    m_position = desc.m_position;
    m_mass = desc.m_mass;
    m_damping = desc.m_dampForce;
    m_physicsBoundType = desc.m_boundBoxDesc.m_boundType;

    if(m_mass >= 999)
    {
        SetInverseMass(0);
        m_mass = 1;
    }
    else
    {
        m_inverseMass = 1 / m_mass;
    }
}

IPhysicsFeature::~IPhysicsFeature()
{
}

void IPhysicsFeature::Update(f32 dt)
{
}

void IPhysicsFeature::Draw(RenderContext& context)
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
