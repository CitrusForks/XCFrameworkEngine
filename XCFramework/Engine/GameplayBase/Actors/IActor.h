/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include <atomic>

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
        ActorState_Initialize,
        ActorState_MetaDataLoad,
        ActorState_Loading,
        ActorState_Loaded,
        ActorState_Ready,
        ActorState_Unloading,
        ActorState_Unloaded,
        ActorState_Destroy
    };

    enum ActorReturnState
    {
        ActorReturnState_Success,
        ActorReturnState_Processing,
        ActorReturnState_Error
    };

    IActor(void);
    virtual ~IActor(void);

    virtual ActorReturnState    Init();

    virtual ActorReturnState    LoadMetaData(const void* metaData);
    virtual ActorReturnState    Load();
    virtual ActorReturnState    OnLoaded();

    virtual ActorReturnState    Update(f32 dt);
    virtual bool                Draw(RenderContext& renderContext) override;

    virtual ActorReturnState    Unload();
    virtual ActorReturnState    OnUnloaded();

    virtual ActorReturnState    Destroy();

    bool                        IsActorLoaded() { return m_actorLoaded; }
    void                        SetActorLoaded(bool isReady) { m_actorLoaded = isReady; }

    void                        Invalidate();
    bool                        IsInvalidated() const { return m_invalidated; }

    void                        SetActorState(ActorState state) { m_actorState = state; }
    ActorState                  GetActorState() const { return m_actorState; }

    const void*                 GetMetaData() const { return m_metaData; }
    void                        SetMetaData(const void* metaData) { m_metaData = metaData; }

protected:

    std::string                 m_userFriendlyName;
    Material                    m_material;

    //Actor states
    std::atomic<bool>           m_actorLoaded;
    std::atomic<ActorState>     m_actorState;
    bool                        m_invalidated;

    const void*                 m_metaData;
};