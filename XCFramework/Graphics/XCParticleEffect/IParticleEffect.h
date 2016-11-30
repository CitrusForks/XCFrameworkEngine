/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/RenderContext.h"
#include "Graphics/IRenderableObject.h"

class IParticleEffect : public IRenderableObject
{
public:
    IParticleEffect();
    virtual ~IParticleEffect();

    virtual void       Update(f32 dt);
    virtual bool       Draw(RenderContext& renderContext) { return true; }
                       
    virtual bool       IsAlive() const                        { return m_isAlive; }
    virtual void       SetAlive(bool alive)                   { m_isAlive = alive; }

protected:

    bool               m_isAlive;
    f32                m_lifeSpan;
    f32                m_timeFromSpawn;
};

