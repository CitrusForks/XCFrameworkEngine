/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class IPhysicsFeature;

class ParticleContact
{
public:
    ParticleContact() {}
    ~ParticleContact(void);

    void                        ContactResolve(IPhysicsFeature* p1, IPhysicsFeature* p2, f32 restitution, f32 penetration, const XCVec4& contactNormal);
    f32                         CalculateSeparatingVelocity();
    void                        ResolveVelocity();
    void                        ResolvePenetration();
    void                        ResolveDragging(IPhysicsFeature* p1, IPhysicsFeature* p2, f32 restitution, f32 penetration, const XCVec4& contactNormal);
    void                        ApplyImpulse(IPhysicsFeature* p1, const XCVec4& units);

private:

    IPhysicsFeature             *m_pParticle1, *m_pParticle2;
    f32                         m_restitution;
    f32                         m_penetration;
    XCVec4                      m_contactNormal;
};