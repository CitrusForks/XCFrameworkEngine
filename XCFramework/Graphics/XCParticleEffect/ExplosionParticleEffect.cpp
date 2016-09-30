/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "ExplosionParticleEffect.h"

ExplosionParticleEffect::ExplosionParticleEffect(XCVec3Unaligned explosionCentre, u32 noOfParticles, f32 lifeSpan)
{
    m_explosionCentre = explosionCentre;
    m_lifeSpan = lifeSpan;
    m_noOfParticles = noOfParticles;

    AssignParticleLocations();
}

ExplosionParticleEffect::~ExplosionParticleEffect()
{
    m_noOfParticles = 0;
    m_pointVertices.clear();
    m_particlesPhysics.clear();
}

void ExplosionParticleEffect::AssignParticleLocations()
{
    XCVec3Unaligned initialVelocity = XCVec3Unaligned(0.0f, 0.0f, 0.0f);

    for (u32 particleIndex = 0; particleIndex < m_noOfParticles; ++particleIndex)
    {
        VertexPosNormTex point(m_explosionCentre, XCVec3Unaligned(0.33f, 0.33f, 0.33f), XCVec2Unaligned(0.0f, 0.0f));
        m_pointVertices.push_back(point);

        //Compute random velocity for a particle.
        f32 randomNegDirection = rand() % 2 ? -1.0f : 1.0f;

        initialVelocity.x = (f32) randomNegDirection * (rand() % 10) / 10;
        initialVelocity.y = (f32) randomNegDirection * (rand() % 10) / 10;
        initialVelocity.z = (f32) randomNegDirection * (rand() % 10) / 10;

        XCVec4 norm = VectorNormalize<3>(XCVec4(initialVelocity.x, initialVelocity.z, initialVelocity.z, 0.0f));

        ParticlePhysics particle = { XCVec4(initialVelocity.x, initialVelocity.y, initialVelocity.z, 0.0f), (f32)(rand() % 5)};
        m_particlesPhysics.push_back(particle);
    }
}

void ExplosionParticleEffect::Update(f32 dt)
{
    IParticleEffect::Update(dt);

    const f32 DRAG_FORCE = 0.1f;

    for (u32 particleIndex = 0; particleIndex < m_noOfParticles; ++particleIndex)
    {
        //Update every particle vertex position as per it's velocity
        //Another way can be compute translation matrix and translate the constant vertex buffer to the world, but every particle would require a diff world?
        ParticlePhysics& particle = m_particlesPhysics[particleIndex];
        VertexPosNormTex& pointVertex = m_pointVertices[particleIndex];

        XCVec4 particlePosition(pointVertex.Pos.x, pointVertex.Pos.y, pointVertex.Pos.z, 0.0f);

        particlePosition += (particle.velocity * particle.acceleration);

        pointVertex.Pos.x = particlePosition[0];
        pointVertex.Pos.y = particlePosition[1];
        pointVertex.Pos.z = particlePosition[2];

        if (particle.acceleration > 0)
        {
            particle.acceleration -= DRAG_FORCE;
        }
    }
}

void ExplosionParticleEffect::Draw(RenderContext& context)
{
}