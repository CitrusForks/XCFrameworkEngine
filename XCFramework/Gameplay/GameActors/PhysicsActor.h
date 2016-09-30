/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/AnimatedActor.h"
#include "Gameplay/XCPhysics/CollisionDetectionTypes.h"
#include "Gameplay/AI/INavigator.h"
#include "Gameplay/XCPhysics/XPhysics.h"

#include "Graphics/XCMesh/XCMesh.h"
#include "Graphics/BasicGeometry/RenderableOBB.h"


class PhysicsActor : public AnimatedActor, public XPhysics, public INavigator
{
public:
    DECLARE_OBJECT_CREATION(PhysicsActor)

    PhysicsActor();
    virtual ~PhysicsActor(void);
    
    //To make a Physics Object Immovable, set Inverse Mass to 0
    virtual void                Init(i32 actorId)             override;
    virtual void                PreLoad(const void* fbBuffer) override   { AnimatedActor::PreLoad(fbBuffer); }
    virtual void                Load()                        override;
    virtual void                UpdateState()                 override;
    virtual void                Update(f32 dt)              override;
    virtual void                Draw(RenderContext& context)  override;
    virtual void                Unload()                      override;
    virtual void                Destroy()                     override;
    
    virtual	void                SetInitialPhysicsProperties();

    OrientedBoundingBox*        GetBoundBox()                            { return (OrientedBoundingBox*)m_boundBox; }
    ECollisionDetectionType     GetCollisionDetectionType()              { return m_collisionDetectionType; }

    void                        SetSecondaryLook(XCVec4& look)   { m_secondaryLookAxis = look; }
    void                        SetSecondaryUp(XCVec4& up)       { m_secondaryLookAxis = up; }
    void                        SetSecondaryRight(XCVec4& right) { m_secondaryLookAxis = right; }

protected:
    ResourceHandle*             m_pMesh;
    ECollisionDetectionType     m_collisionDetectionType;

    RenderableOBB*              m_boundBox;

    XCVec4                      m_secondaryLookAxis;
    XCVec4                      m_secondaryUpAxis;
    XCVec4                      m_secondaryRightAxis;
};