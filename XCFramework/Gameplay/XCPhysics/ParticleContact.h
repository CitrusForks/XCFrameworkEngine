/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/PhysicsActor.h"

class ParticleContact
{

public:
    ParticleContact() {}
    ~ParticleContact(void);

    void                        ContactResolve(PhysicsActor* p1, PhysicsActor* p2, float restitution, float penetration, XCVecIntrinsic4 contactNormal);
    float                       CalculateSeparatingVelocity();
    void                        ResolveVelocity();
    void                        ResolvePenetration();
    void                        ResolveDragging(PhysicsActor* p1, PhysicsActor* p2, float restitution, float penetration, XCVecIntrinsic4 contactNormal);
    void                        ApplyImpulse(PhysicsActor* p1, XCVecIntrinsic4 units);

private:

    PhysicsActor               *m_pParticle1, *m_pParticle2;
    float                       m_restitution;
    float                       m_penetration;
    XCVecIntrinsic4             m_contactNormal;
};