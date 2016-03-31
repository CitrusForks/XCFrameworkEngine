/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IParticleEffect.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"

class ExplosionParticleEffect : public IParticleEffect
{
public:
    ExplosionParticleEffect(XCVec3Unaligned explosionCentre, unsigned int noOfParticles, float lifeSpan);
    ~ExplosionParticleEffect();

    void                            Update(float dt);
    void                            Draw(RenderContext& context);

    void                            AssignParticleLocations();

private:

    struct ParticlePhysics
    {
        XCVecIntrinsic4     velocity;
        float               acceleration;
    };
    
    //Vertex Buffer
    std::vector<VertexPosNormTex>   m_pointVertices;

    //For every vertex in vertex buffer, physics properties, couldn't combine both, too inefficient.
    std::vector<ParticlePhysics>    m_particlesPhysics;
    
    XCVec3Unaligned                 m_explosionCentre;
    unsigned int                    m_noOfParticles;
};