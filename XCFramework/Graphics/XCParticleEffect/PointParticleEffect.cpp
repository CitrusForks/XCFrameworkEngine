/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "PointParticleEffect.h"

PointParticleEffect::PointParticleEffect(XCVec3Unaligned& position, const f32 lifeSpan)
{
    m_particlePosition = position;
    m_lifeSpan = lifeSpan;

    m_pointVertex = { m_particlePosition.x,
                      m_particlePosition.y,
                      m_particlePosition.z,
                    };
}

PointParticleEffect::~PointParticleEffect()
{
    m_particlePosition = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_lifeSpan = 0.0f;
    m_pointVertex = { 0 };
}

void PointParticleEffect::Update(f32 dt)
{
    IParticleEffect::Update(dt);
}

bool PointParticleEffect::Draw(RenderContext& context)
{
    return true;
}