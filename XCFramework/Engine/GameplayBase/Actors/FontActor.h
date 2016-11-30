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
    ~FontActor(void);
    
    virtual IActor::ActorReturnState Init() override;
    virtual IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    virtual IActor::ActorReturnState Load() override;
    virtual IActor::ActorReturnState Update(f32 dt) override;
    virtual bool                     Draw(RenderContext& renderContext) override;
    virtual IActor::ActorReturnState Destroy() override;
    
protected:
    ResourceHandle*                     m_fontMesh;
};