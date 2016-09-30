/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/System/ISystem.h"
#include "IParticleEffect.h"

class ParticleEffectSystem : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(ParticleEffectSystem)

    ParticleEffectSystem();
    ~ParticleEffectSystem();

    void      Update(f32 dt);
    void      Draw(RenderContext& renderContext);

    void      AddParticleEffect(IParticleEffect* effect);
    void      DestroyAllParticleEffects();

private:
    std::vector<IParticleEffect*>        m_instancedParticleEffects;
};