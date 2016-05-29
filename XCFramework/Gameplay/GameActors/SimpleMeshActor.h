/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/SimpleActor.h"

class ResourceHandle;

class SimpleMeshActor : public SimpleActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleMeshActor)

    SimpleMeshActor();
    virtual ~SimpleMeshActor(void);

    virtual void                Init(int actorId);
    virtual void                PreLoad(const void* fbBuffer) { SimpleActor::PreLoad(fbBuffer); }
    virtual void                Update(float dt);
    virtual void                Draw(RenderContext& renderContext);
    virtual void                Destroy();

protected:
    ResourceHandle*             m_pMesh;
};