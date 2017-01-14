/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Physics/IPhysicsFeature.h"

class RigidBody : public IPhysicsFeature
{
public:
    RigidBody() {}
    ~RigidBody();

    void                Init(const PhysicsDesc& desc) override;
    void                Update(f32 dtS) override;

    void                ClearForce();
    void                ClearVelocity();

    void                AddForce(const XCVec4& newForce);
    
    XCVec4              GetVelocity() { return m_velocity; }
    void                SetVelocity(const XCVec4& vel) { m_velocity = vel; }

    XCVec4              GetContactNormal() const { return m_ContactNormal; }

    void                SetContactNormal(XCVec4& newValue) { m_ContactNormal = newValue; }

private:
    XCVec4              m_velocity;
    XCVec4              m_Acceleration;
    XCVec4              m_ForceAccumulator;
    XCVec4              m_ContactNormal;
};