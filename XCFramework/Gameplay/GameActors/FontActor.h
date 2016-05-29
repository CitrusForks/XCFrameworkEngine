/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IActor.h"
#include "Engine/Resource/ResourceHandle.h"

class FontActor : public IActor
{
public:
    DECLARE_OBJECT_CREATION(FontActor)

    FontActor();
    virtual ~FontActor(void);
    
    virtual void                        Init(int actorId) override;
    virtual void                        PreLoad(const void* fbBuffer) override;
    virtual void                        Load() override;
    virtual void                        UpdateState() override;
    virtual void                        Update(float dt) override;
    virtual void                        Draw(RenderContext& context) override;
    virtual void                        Destroy() override;
    
protected:
    ResourceHandle*                     m_fontMesh;
};