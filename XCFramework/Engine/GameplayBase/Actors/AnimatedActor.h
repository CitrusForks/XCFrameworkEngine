/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IActor.h"

class AnimatedActor : public IActor
{
public:
    DECLARE_OBJECT_CREATION(AnimatedActor)

    AnimatedActor(void);
    virtual ~AnimatedActor(void);

    virtual void                    Init(i32 actorId);
    virtual void                    PreLoad(const void* fbBuffer) { IActor::PreLoad(fbBuffer); }
    virtual void                    Update(f32 dt);
    virtual void                    Draw(RenderContext& context);
    virtual void                    Destroy();
};