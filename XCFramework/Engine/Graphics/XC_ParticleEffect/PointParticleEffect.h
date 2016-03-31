/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IParticleEffect.h"

class PointParticleEffect : public IParticleEffect
{
public:
    PointParticleEffect(XCVec3Unaligned& position, const float lifeSpan);
    ~PointParticleEffect();

    void                            Update(float dt);
    void                            Draw(RenderContext& context);

private:
    
    struct PointVertexColor
    {
        float       x, y, z;
    };

    XCVec3Unaligned                 m_particlePosition;
    PointVertexColor                m_pointVertex;
};