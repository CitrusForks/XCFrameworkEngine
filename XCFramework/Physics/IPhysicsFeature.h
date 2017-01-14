/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "PhysicsDesc.h"

class IPhysicsBoundVolume;

class IPhysicsFeature
{
public:
    IPhysicsFeature();
    virtual ~IPhysicsFeature();

    virtual void            Init(const PhysicsDesc& desc);
    virtual void            Update(f32 dtS);

    bool                    HasFiniteMass() const;
    f32                     GetMass() const;
    f32                     GetInverseMass() const { return m_inverseMass; }

    XCVec4                  GetTransformedPosition() const { return m_position; }
    void                    SetTransformedPosition(XCVec4& pos) { m_position = pos; }

    void                    SetBoundType(IPhysicsBoundVolume* bound);
    PhysicsBoundType        GetBoundType() const;

    IPhysicsBoundVolume*    GetBoundVolume() { return m_physicsBoundVolume; }

    void                    SetInverseMass(f32 inverseMass) { m_inverseMass = inverseMass; }

    bool                    IsDirty() const { return m_isDirty; }
    void                    SetDirty() { m_isDirty = true; }

    template<class Type>
    Type*                   GetTyped() { return static_cast<Type*>(this); }

protected:
    XCVec4                  m_position;
    XCVec4                  m_orientation;
    f32                     m_damping;
    f32                     m_inverseMass;
    f32                     m_mass;

    bool                    m_isDirty;
    IPhysicsBoundVolume*    m_physicsBoundVolume;
};