/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "SimpleActor.h"

class ResourceHandle;

class SimpleMeshActor : public SimpleActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleMeshActor)

    SimpleMeshActor();
    virtual ~SimpleMeshActor(void);

    virtual IActor::ActorReturnState    Init();
    virtual IActor::ActorReturnState    LoadMetaData(const void* metaData) { return SimpleActor::LoadMetaData(metaData); }
    virtual IActor::ActorReturnState    OnLoaded() override { return SimpleActor::OnLoaded(); }
    virtual IActor::ActorReturnState    Update(f32 dt);
    virtual bool                        Draw(RenderContext& renderContext);
    virtual IActor::ActorReturnState    Destroy();

protected:
    ResourceHandle*                     m_pMesh;
};