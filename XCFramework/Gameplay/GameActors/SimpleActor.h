/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/iactor.h"

class SimpleActor : public IActor
{
public:
    SimpleActor();
    virtual ~SimpleActor(void);

    virtual void                    Init(int actorId) override;
    virtual void                    PreLoad(const void* fbBuffer) override { IActor::PreLoad(fbBuffer); }
    virtual void                    UpdateState() override { IActor::UpdateState(); }
    virtual void                    Update(float dt) override;
    virtual void                    Draw(RenderContext& renderContext) override;
    virtual void                    Destroy() override;
};