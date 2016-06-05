/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/AnimatedActor.h"
#include "Gameplay/XCPhysics/CollisionDetectionTypes.h"
#include "AI/INavigator.h"

#include "Graphics/XC_Mesh/XCMesh.h"
#include "Graphics/BasicGeometry/RenderableOBB.h"

#include "XCPhysics/XPhysics.h"

class PhysicsActor : public AnimatedActor, public XPhysics, public INavigator
{
public:
    DECLARE_OBJECT_CREATION(PhysicsActor)

    PhysicsActor();
    virtual ~PhysicsActor(void);
    
    //To make a Physics Object Immovable, set Inverse Mass to 0
    virtual void                Init(int actorId)             override;
    virtual void                PreLoad(const void* fbBuffer) override   { AnimatedActor::PreLoad(fbBuffer); }
    virtual void                Load()                        override;
    virtual void                UpdateState()                 override;
    virtual void                Update(float dt)              override;
    virtual void                Draw(RenderContext& context)  override;
    virtual void                Unload()                      override;
    virtual void                Destroy()                     override;
    
    virtual	void                SetInitialPhysicsProperties();

    OrientedBoundingBox*        GetBoundBox()                            { return (OrientedBoundingBox*)m_boundBox; }
    ECollisionDetectionType     GetCollisionDetectionType()              { return m_collisionDetectionType; }

    void                        SetSecondaryLook(XCVecIntrinsic4 look)   { m_secondaryLookAxis = look; }
    void                        SetSecondaryUp(XCVecIntrinsic4 up)       { m_secondaryLookAxis = up; }
    void                        SetSecondaryRight(XCVecIntrinsic4 right) { m_secondaryLookAxis = right; }

protected:
    ResourceHandle*             m_pMesh;
    ECollisionDetectionType     m_collisionDetectionType;

    RenderableOBB*              m_boundBox;

    XCVecIntrinsic4             m_secondaryLookAxis;
    XCVecIntrinsic4             m_secondaryUpAxis;
    XCVecIntrinsic4             m_secondaryRightAxis;
};