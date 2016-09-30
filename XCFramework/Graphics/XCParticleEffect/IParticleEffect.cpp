/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "IParticleEffect.h"

IParticleEffect::IParticleEffect()
{
    m_lifeSpan = 0.0f;
    SetAlive(true);
    m_timeFromSpawn = 0.0f;
}

IParticleEffect::~IParticleEffect()
{
}

void IParticleEffect::Update(f32 dt)
{
    m_timeFromSpawn += dt;

    if (m_timeFromSpawn > m_lifeSpan)
    {
        SetAlive(false);
    }
}