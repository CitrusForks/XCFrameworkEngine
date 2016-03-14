/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include<limits>

class XPhysics
{
public:
    static bool s_enableGravity;

    XPhysics();
    ~XPhysics();

    void                InitXPhysics(XCVecIntrinsic4 _position, XCVecIntrinsic4 _velocity, XCVecIntrinsic4 _acceleration, float _mass, float _damping);
    void                Integrator(float dtS);
    void                ClearForce();
    void                AddForce(XCVecIntrinsic4 _newForce);
    bool                HasFiniteMass();

    float               GetMass();
    float               GetInverseMass() { return m_InverseMass; }
    XCVecIntrinsic4     GetVelocity() { return m_Velocity; }

    XCVecIntrinsic4     GetTransformedPosition() { return m_Position; }
    void                SetTransformedPosition(XCVecIntrinsic4 pos) { m_Position = pos; }

    XCVecIntrinsic4     GetContactNormal() { return m_ContactNormal; }

    void                SetInverseMass(float _inverseMass) { m_InverseMass = _inverseMass; }
    void                SetVelocity(XCVecIntrinsic4 _newValue) { m_Velocity = _newValue; }
    void                SetContactNormal(XCVecIntrinsic4 _newValue) { m_ContactNormal = _newValue; }

protected:
    XCVecIntrinsic4     m_Velocity;
    XCVecIntrinsic4     m_Position;
    XCVecIntrinsic4     m_Acceleration;
    XCVecIntrinsic4     m_ForceAccumulator;
    XCVecIntrinsic4     m_GAcceleration;
    XCVecIntrinsic4     m_ContactNormal;

    float               m_Damping;
    float               m_InverseMass;
    float               m_Mass;
};