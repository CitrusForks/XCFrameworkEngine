/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

class IPhysicsFeature
{
public:
    IPhysicsFeature();
    virtual ~IPhysicsFeature();

    void                InitXPhysics(const XCVec4& position, const XCVec4& velocity, const XCVec4& acceleration, f32 mass, f32 damping);
    void                Integrator(f32 dtS);
    void                ClearForce();
    void                ClearVelocity();
    void                AddForce(const XCVec4& newForce);
    bool                HasFiniteMass() const;

    f32                 GetMass() const;
    f32                 GetInverseMass() const { return m_InverseMass; }
    XCVec4              GetVelocity() { return m_Velocity; }

    XCVec4              GetTransformedPosition() const { return m_Position; }
    void                SetTransformedPosition(XCVec4& pos) { m_Position = pos; }

    XCVec4              GetContactNormal() const { return m_ContactNormal; }

    void                SetInverseMass(f32 _inverseMass) { m_InverseMass = _inverseMass; }
    void                SetVelocity(XCVec4& _newValue) { m_Velocity = _newValue; }
    void                SetContactNormal(XCVec4& _newValue) { m_ContactNormal = _newValue; }

protected:
    XCVec4              m_Velocity;
    XCVec4              m_Position;
    XCVec4              m_Acceleration;
    XCVec4              m_ForceAccumulator;
    XCVec4              m_ContactNormal;

    f32                 m_Damping;
    f32                 m_InverseMass;
    f32                 m_Mass;
};