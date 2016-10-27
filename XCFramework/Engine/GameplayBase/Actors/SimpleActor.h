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

    virtual void                    Init(i32 actorId) override;
    virtual void                    PreLoad(const void* fbBuffer) override { IActor::PreLoad(fbBuffer); }
    virtual void                    Load() override { IActor::Load(); }
    virtual void                    UpdateState() override { IActor::UpdateState(); }
    virtual void                    Update(f32 dt) override;
    virtual void                    Draw(RenderContext& renderContext) override;
    virtual void                    Destroy() override;
};