/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IParticleEffect.h"
#include "Graphics/XCShaders/XCVertexFormat.h"

class ExplosionParticleEffect : public IParticleEffect
{
public:
    ExplosionParticleEffect(XCVec3Unaligned explosionCentre, u32 noOfParticles, f32 lifeSpan);
    ~ExplosionParticleEffect();

    void                            Update(f32 dt) override;
    bool                            Draw(RenderContext& context) override;

    void                            AssignParticleLocations();

private:

    struct ParticlePhysics
    {
        XCVec4     velocity;
        f32        acceleration;
    };
    
    //Vertex Buffer
    std::vector<VertexPosNormTex>   m_pointVertices;

    //For every vertex in vertex buffer, physics properties, couldn't combine both, too inefficient.
    std::vector<ParticlePhysics>    m_particlesPhysics;
    
    XCVec3Unaligned                 m_explosionCentre;
    u32                             m_noOfParticles;
};