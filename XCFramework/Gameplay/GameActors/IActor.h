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

    IActor(void);
    virtual ~IActor(void);

    virtual             void        PreLoad(const void* fbBuffer) {}
    virtual             void        Load();
    virtual             void        Update(float dt);
    virtual             void        Draw(RenderContext& renderContext);
    virtual             void        Destroy();

    virtual             void        Invalidate();
    virtual             bool        IsWorldReady()                   { return m_worldReady; }
    virtual             void        SetWorldReady(bool isReady)      { m_worldReady = isReady; }

    bool                            IsInvalidated()                  { return m_invalidated; }

protected:
    std::string                     m_userFriendlyName;
    EGameActorType                  m_actorType;

    bool                            m_worldReady;
    bool                            m_invalidated;
};