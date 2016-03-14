/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/AnimatedActor.h"
#include "Gameplay/XCPhysics/XPhysics.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Gameplay/XCPhysics/CollisionDetectionTypes.h"
#include "Gameplay/AI/INavigator.h"


class PhysicsActor : public AnimatedActor, public XPhysics, public INavigator
{
public:
    DECLARE_OBJECT_CREATION(PhysicsActor);

    PhysicsActor();
    virtual ~PhysicsActor(void);
    
    //To make a Physics Object Immovable, set Inverse Mass to 0
    virtual void                        Init(int actorId);
    virtual void                        PreLoad(const void* fbBuffer) { AnimatedActor::PreLoad(fbBuffer); }
    virtual void                        Load();
    virtual	void                        SetInitialPhysicsProperties();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Invalidate();
    virtual void                        Destroy();
    
    XCMesh*                             getMesh()               { return m_pMesh;  }
    OrientedBoundingBox*                getBoundBox()           { return m_boundBox; }

    ECollisionDetectionType             getCollisionDetectionType()                 { return m_collisionDetectionType; }

    void                                SetSecondaryLook(XCVecIntrinsic4 look)             { m_secondaryLookAxis = look; }
    void                                SetSecondaryUp(XCVecIntrinsic4 up)                 { m_secondaryLookAxis = up; }
    void                                SetSecondaryRight(XCVecIntrinsic4 right)           { m_secondaryLookAxis = right; }

protected:
    XCMesh*                             m_pMesh;
    ECollisionDetectionType             m_collisionDetectionType;

    OrientedBoundingBox*                m_boundBox;

    XCVecIntrinsic4                            m_secondaryLookAxis;
    XCVecIntrinsic4                            m_secondaryUpAxis;
    XCVecIntrinsic4                            m_secondaryRightAxis;
};