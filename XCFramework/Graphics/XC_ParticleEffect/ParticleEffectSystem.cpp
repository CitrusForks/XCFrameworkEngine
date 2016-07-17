/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "ParticleEffectSystem.h"

ParticleEffectSystem::ParticleEffectSystem()
{
}

ParticleEffectSystem::~ParticleEffectSystem()
{
    DestroyAllParticleEffects();
}

void ParticleEffectSystem::AddParticleEffect(IParticleEffect* effect)
{
    if (effect != nullptr)
    {
        m_instancedParticleEffects.push_back(effect);
    }
}

void ParticleEffectSystem::Update(f32 dt)
{
    u32 i = 0;
    while (i < m_instancedParticleEffects.size())
    {
        if (m_instancedParticleEffects[i]->IsAlive())
        {
            m_instancedParticleEffects[i]->Update(dt);
            ++i;
        }
        else
        {
            delete(m_instancedParticleEffects[i]);
            m_instancedParticleEffects.erase(m_instancedParticleEffects.begin() + i);
        }
    }
}

void ParticleEffectSystem::Draw(RenderContext& renderContext)
{
    for (auto& pEffect : m_instancedParticleEffects)
    {
        pEffect->Draw(renderContext);
    }
}

void ParticleEffectSystem::DestroyAllParticleEffects()
{
    while (m_instancedParticleEffects.size() != 0)
    {
        delete(m_instancedParticleEffects.back());
        m_instancedParticleEffects.pop_back();
    }

    m_instancedParticleEffects.clear();
}