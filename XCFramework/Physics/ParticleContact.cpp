/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "IPhysicsFeature.h"
#include "ParticleContact.h"

#include "Phusike/RigidBody.h"

ParticleContact::~ParticleContact(void)
{
}

void ParticleContact::ContactResolve(IPhysicsFeature* p1, IPhysicsFeature* p2, f32 restitution, f32 penetration, const XCVec4& contactNormal)
{
    m_pParticle1 = p1;
    m_pParticle2 = p2;
    m_restitution = restitution;
    m_contactNormal = contactNormal;

    ResolveVelocity();
    ResolvePenetration();
}

f32 ParticleContact::CalculateSeparatingVelocity()
{
    XCVec4 relativeVelocity = m_pParticle1->GetTyped<RigidBody>()->GetVelocity();

    //This is required, when the particle 1 is stationary and non movable, reverse the particles and calculate separating velocity based on particle 2.
    if (IsVectorEqual(relativeVelocity, XCVec4()) && m_pParticle2 != nullptr)
    {
        relativeVelocity = m_pParticle2->GetTyped<RigidBody>()->GetVelocity();

        //Swap the Particles
        IPhysicsFeature* temp = m_pParticle1;
        m_pParticle1 = m_pParticle2;
        m_pParticle2 = temp;
    }

    return VectorDot(relativeVelocity, m_contactNormal);
}

void ParticleContact::ApplyImpulse(IPhysicsFeature* p1, const XCVec4& impulse)
{
    XCVec4 currentPos = p1->GetTransformedPosition();
    currentPos.Set<Y>(impulse.Get<Y>());
    p1->SetTransformedPosition(currentPos);
    //Not necessary, calculating now from ResolveVelocity() itself.
    //p1->AddForce(XMLoadFloat3(&XMFLOAT3(0, (float)5.0001, 0)));
}

void ParticleContact::ResolveVelocity()
{
    f32 separatingVelocity = CalculateSeparatingVelocity();

    if(separatingVelocity > 0)
    {
        return;
    }

    f32 newSeparatingVelocity = -separatingVelocity * m_restitution;

    f32 deltaVelocity = newSeparatingVelocity - separatingVelocity;

    f32 totalInverseMass = m_pParticle1->GetInverseMass();

    if (m_pParticle2 != nullptr)
    {
        totalInverseMass += m_pParticle2->GetInverseMass();
    }

    if(totalInverseMass <= 0)
        return;

    f32 impulse = deltaVelocity / totalInverseMass;

    XCVec4 impulsePerIMass = m_contactNormal * impulse;

    XCVec4 particle1Velocity = m_pParticle1->GetTyped<RigidBody>()->GetVelocity();

    XCVec4 particle2Velocity;
    if (m_pParticle2 != nullptr)
    {
        particle2Velocity = m_pParticle2->GetTyped<RigidBody>()->GetVelocity();
    }

    particle1Velocity += impulsePerIMass * m_pParticle1->GetInverseMass();
    m_pParticle1->GetTyped<RigidBody>()->SetVelocity(particle1Velocity);

    //Logger("[Contact Resolve] Impulse : %f, \n", XMVectorGetY(impulsePerIMass));

    if(m_pParticle2 != nullptr)
    {
        particle2Velocity += impulsePerIMass * -m_pParticle2->GetInverseMass();
        m_pParticle2->GetTyped<RigidBody>()->SetVelocity(particle2Velocity);
    }
}

void ParticleContact::ResolvePenetration()
{
    //No penetation required as of now
    if(m_penetration <= 0)
        return;
}

void ParticleContact::ResolveDragging(IPhysicsFeature* p1, IPhysicsFeature* p2, f32 restitution, f32 penetration, const XCVec4& contactNormal)
{
    m_pParticle1        =   p1;
    m_pParticle2        =   p2;
    m_restitution       =   restitution;
    m_contactNormal     =   contactNormal;

    p1->GetTyped<RigidBody>()->ClearForce();
    p1->GetTyped<RigidBody>()->AddForce(contactNormal * restitution);
}
