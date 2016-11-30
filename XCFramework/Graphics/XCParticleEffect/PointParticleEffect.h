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
    PointParticleEffect(XCVec3Unaligned& position, const f32 lifeSpan);
    ~PointParticleEffect();

    void                            Update(f32 dt) override;
    bool                            Draw(RenderContext& context) override;

private:
    
    struct PointVertexColor
    {
        f32       x, y, z;
    };

    XCVec3Unaligned                 m_particlePosition;
    PointVertexColor                m_pointVertex;
};