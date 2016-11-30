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

    virtual IActor::ActorReturnState Init() override;

    virtual IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    virtual IActor::ActorReturnState Load() override;
    virtual IActor::ActorReturnState OnLoaded() override;
    
    virtual IActor::ActorReturnState Update(f32 dt) override;
    virtual bool                     Draw(RenderContext& renderContext) override;
    
    virtual IActor::ActorReturnState Unload() override;
    virtual IActor::ActorReturnState OnUnloaded() override;

    virtual IActor::ActorReturnState Destroy() override;
};