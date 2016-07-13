/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class XPhysics
{
public:
    static bool s_enableGravity;

    XPhysics();
    ~XPhysics();

    void                InitXPhysics(XCVec4& position, XCVec4& velocity, XCVec4& acceleration, float mass, float damping);
    void                Integrator(float dtS);
    void                ClearForce();
    void                ClearVelocity();
    void                AddForce(XCVec4& newForce);
    bool                HasFiniteMass();

    float               GetMass();
    float               GetInverseMass() { return m_InverseMass; }
    XCVec4              GetVelocity() { return m_Velocity; }

    XCVec4              GetTransformedPosition() { return m_Position; }
    void                SetTransformedPosition(XCVec4& pos) { m_Position = pos; }

    XCVec4              GetContactNormal() { return m_ContactNormal; }

    void                SetInverseMass(float _inverseMass) { m_InverseMass = _inverseMass; }
    void                SetVelocity(XCVec4& _newValue) { m_Velocity = _newValue; }
    void                SetContactNormal(XCVec4& _newValue) { m_ContactNormal = _newValue; }

protected:
    XCVec4     m_Velocity;
    XCVec4     m_Position;
    XCVec4     m_Acceleration;
    XCVec4     m_ForceAccumulator;
    XCVec4     m_GAcceleration;
    XCVec4     m_ContactNormal;

    float               m_Damping;
    float               m_InverseMass;
    float               m_Mass;
};