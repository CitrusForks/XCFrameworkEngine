/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Serializer/IBase.h"
#include "Engine/ObjectBase/IObjectDimensions.h"
#include "Engine/Graphics/IRenderableObject.h"

#include "Gameplay/GameActors/GameActorTypes.h"

class IActor : public IBase, public IObjectDimensions, public IRenderableObject
{
public:
    DECLARE_OBJECT_CREATION(IActor)

    enum ActorState
    {
        ActorState_None,
        ActorState_Loading,
        ActorState_Loaded,
        ActorState_Unloaded
    };

    IActor(void);
    virtual ~IActor(void);

    virtual void        PreLoad(const void* fbBuffer);
    virtual void        Load();
    virtual void        UpdateState();
    virtual void        Update(float dt);
    virtual void        Draw(RenderContext& renderContext);
    virtual void        Unload();
    virtual void        Destroy();
    
    void                SetWorldReady(bool isReady)      { m_worldReady = isReady; }
    bool                IsWorldReady()                   { return m_worldReady; }

    void                Invalidate();
    bool                IsInvalidated()                  { return m_invalidated; }

    ActorState          GetActorState()                  { return m_actorState; }

protected:

    std::string         m_userFriendlyName;
    EGameActorType      m_actorType;
                        
    bool                m_worldReady;
    bool                m_invalidated;
    ActorState          m_actorState;
};