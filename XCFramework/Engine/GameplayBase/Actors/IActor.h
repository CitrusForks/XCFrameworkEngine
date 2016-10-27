/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/Serializer/IBase.h"

#include "Engine/ObjectBase/IObjectDimensions.h"
#include "Base/DataStructures/Trees/XCTreeNode.h"

#include "Graphics/IRenderableObject.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

class IActor : public IBase, public IObjectDimensions, public IRenderableObject, public NTreeNode<IActor*>
{
public:
    DECLARE_OBJECT_CREATION(IActor)

    enum ActorState
    {
        ActorState_None,
        ActorState_PreLoaded,
        ActorState_Loading,
        ActorState_Loaded,
        ActorState_Unloaded
    };

    IActor(void);
    virtual ~IActor(void);

    virtual void        PreLoad(const void* fbBuffer);
    virtual void        Load();
    virtual void        UpdateState();
    virtual void        Update(f32 dt);
    virtual void        Draw(RenderContext& renderContext);
    virtual void        Unload();
    virtual void        Destroy();

    bool                IsActorReady()                   { return m_actorReady; }

    void                Invalidate();
    bool                IsInvalidated()                  { return m_invalidated; }

    void                SetActorState(ActorState state)  { m_actorState = state; }
    ActorState          GetActorState()                  { return m_actorState; }

protected:

    std::string          m_userFriendlyName;
    Material             m_material;

    //Actor states
    bool                 m_actorReady;
    bool                 m_invalidated;
    ActorState           m_actorState;
};