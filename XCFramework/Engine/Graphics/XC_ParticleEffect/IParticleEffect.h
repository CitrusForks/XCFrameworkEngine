/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/RenderContext.h"

class IParticleEffect
{
public:
    IParticleEffect();
    virtual ~IParticleEffect();

    virtual void         Update(float dt);
    virtual void         Draw(RenderContext& renderContext) = 0;

    virtual bool         isAlive() const                        { return m_isAlive; }
    virtual void         setAlive(bool alive)                   { m_isAlive = alive; }

protected:

    bool                 m_isAlive;
    float                m_lifeSpan;
    float                m_timeFromSpawn;
};

