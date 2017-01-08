/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "AnimatedActor.h"

#include "Engine/AI/INavigator.h"

#include "Graphics/BasicGeometry/RenderableOBB.h"

class IPhysicsFeature;

class PhysicsActor : public AnimatedActor, public INavigator
{
public:
    DECLARE_OBJECT_CREATION(PhysicsActor)

    PhysicsActor();
    virtual ~PhysicsActor(void);
    
    virtual IActor::ActorReturnState    Init() override;

    virtual IActor::ActorReturnState    LoadMetaData(const void* metaData) override { return AnimatedActor::LoadMetaData(metaData); }
    virtual IActor::ActorReturnState    Load() override;
    virtual IActor::ActorReturnState    OnLoaded() override;

    virtual IActor::ActorReturnState    Update(f32 dt) override;
    virtual bool                        Draw(RenderContext& renderContext) override;

    virtual IActor::ActorReturnState    Unload() override;
    virtual IActor::ActorReturnState    Destroy() override;
    
    virtual void                        SetInitialPhysicsProperties();

    OrientedBoundingBox*                GetBoundBox() { return (OrientedBoundingBox*)m_boundBox; }

    void                                SetSecondaryLook(XCVec4& look)   { m_secondaryLookAxis = look; }
    void                                SetSecondaryUp(XCVec4& up)       { m_secondaryLookAxis = up; }
    void                                SetSecondaryRight(XCVec4& right) { m_secondaryLookAxis = right; }

    //For use with NPC AI
    void                                AddForce(const XCVec4& force);

protected:
    IPhysicsFeature*                    m_physicsFeature;
    ResourceHandle*                     m_pMesh;

    RenderableOBB*                      m_boundBox;

    XCVec4                              m_secondaryLookAxis;
    XCVec4                              m_secondaryUpAxis;
    XCVec4                              m_secondaryRightAxis;
};