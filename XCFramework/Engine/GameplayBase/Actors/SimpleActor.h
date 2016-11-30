/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IActor.h"

class SimpleActor : public IActor
{
public:
    SimpleActor();
    virtual ~SimpleActor(void);

    virtual IActor::ActorReturnState Init() override;
    virtual IActor::ActorReturnState LoadMetaData(const void* metaData) override { return IActor::LoadMetaData(metaData); }
    virtual IActor::ActorReturnState Load() override { return IActor::Load(); }
    virtual IActor::ActorReturnState OnLoaded() override { return IActor::OnLoaded(); }
    virtual IActor::ActorReturnState Update(f32 dt) override;
    virtual bool                     Draw(RenderContext& renderContext) override;
    virtual IActor::ActorReturnState Destroy() override;
};