/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */


#include "PhysicsPrecompiledHeader.h"

#include "PhysicsPlayground.h"
#include "PhysicsDesc.h"
#include "CollisionDetection.h"

#include "Phusike/RigidBody.h"
#include "Phusike/StaticBody.h"

#include "Graphics/RenderContext.h"

//WorldCollisionTask--------------------------------------
void PhysicsCollisionResolverTask::Init()
{
    AsyncTask::Init();
}

void PhysicsCollisionResolverTask::Run()
{
    AsyncTask::Run();
}

void PhysicsCollisionResolverTask::Destroy()
{
    AsyncTask::Destroy();
}


//PhysicsPlayground----------------------------------------
void PhysicsPlayground::Init(TaskManager& taskMgr)
{
    m_taskManager = &taskMgr;

    //Initialize the collision thread
    m_collisionResolverTask = XCNEW(PhysicsCollisionResolverTask)(*this);
    m_collisionResolverTask->SetTaskPriority(THREAD_PRIORITY_HIGHEST);
    m_taskManager->RegisterTask(m_collisionResolverTask);
}

void PhysicsPlayground::Update(float dt)
{
    for (auto& phyFeature : m_physicsFeatures)
    {
        phyFeature->Update(dt);
    }
}

void PhysicsPlayground::Draw(RenderContext& context)
{
    for (auto& phyFeature : m_physicsFeatures)
    {
        phyFeature->Draw(context);
    }
}

void PhysicsPlayground::Destroy()
{
    m_collisionResolverTask->Destroy();
    m_taskManager->UnregisterTask(m_collisionResolverTask->GetThreadId());
    XCDELETE(m_collisionResolverTask);
}

IPhysicsFeature* PhysicsPlayground::CreatePhysicsFeature(const PhysicsDesc& phydesc)
{
    IPhysicsFeature* feature = nullptr;

    //Decide body type
    switch(phydesc.m_bodyType)
    {
        case PhysicsBodyType_RigidDynamic:
            {
                feature = XCNEW(RigidBody)();
                break;
            }

        case PhysicsBodyType_RigidStatic:
            {
                feature = XCNEW(StaticBody)();
                break;
            }

        default:
            XCASSERT(false);
            break;
    }

    //Initialize the feature
    feature->Init(phydesc);

    m_physicsFeatures.push_back(feature);
    return feature;
}

void PhysicsPlayground::RemovePhysicsFeature(IPhysicsFeature* phyActor)
{
    auto& feature = std::find_if(m_physicsFeatures.begin(), m_physicsFeatures.end(), 
        [phyActor](const IPhysicsFeature* obj) -> bool
    {
        return obj == phyActor;
    });

    if (feature != m_physicsFeatures.end())
    {
        m_physicsFeatures.erase(feature);
    }
    else
    {
        XCASSERT(false);
    }
}

void PhysicsPlayground::TestCollision()
{
    if (m_physicsFeatures.size() > 0)
    {
        std::vector<IPhysicsFeature*>::iterator obj1;
        for (obj1 = m_physicsFeatures.begin(); obj1 != m_physicsFeatures.end() - 1; ++obj1)
        {
            std::vector<IPhysicsFeature*>::iterator obj2;
            for (obj2 = obj1 + 1; obj2 != m_physicsFeatures.end(); ++obj2)
            {
                if (*obj1 && *obj2 && CheckCollision(*obj1, *obj2))
                {
                    //Logger("COLLIDED VERY WELL");
                }
            }
        }
    }
}

bool PhysicsPlayground::CheckCollision(IPhysicsFeature* obj1, IPhysicsFeature* obj2)
{
    /*
    switch (obj1->GetCollisionDetectionType() | obj2->GetCollisionDetectionType())
    {
    case PhysicsBoundType_Box | PhysicsBoundType_Box:
        if (obj1->GetBoundBox()->m_TransformedBox.Intersects(obj2->GetBoundBox()->m_TransformedBox))
        {
            //Before resolving know which corner point was hit
            DirectX::XMFLOAT3 points[8];
            obj1->GetBoundBox()->m_TransformedBox.GetCorners(points);

            XCVec4 contactNormal;

            contactNormal = CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(obj1->GetBoundBox(), obj2->GetBoundBox());
            //contactNormal = getContactNormalFromBoundBoxContainedPlane(obj2->getMesh()->getAABoundBox(), obj1->getMesh()->getAABoundBox());
            //Resolve the collision
            m_particleContact.ContactResolve(obj1, obj2, 1.0f, 0.0f, contactNormal);
            return true;
        }
        break;

    case PhysicsBoundType_Box | PhysicsBoundType_TriangleMesh:

        break;

    case PhysicsBoundType_HeightField | PhysicsBoundType_Box:
    {
        //Handle terrain and boundbox collision
        IPhysicsFeature* bboxActor = obj1->GetCollisionDetectionType() == PhysicsBoundType_Box ? obj1 : obj2;
        IPhysicsFeature* terrainActor = obj1->GetCollisionDetectionType() == PhysicsBoundType_Terrain ? obj1 : obj2;

        XCVec4 contactPoint = ((Terrain*)terrainActor)->CheckTerrainCollisionFromPoint(bboxActor->GetBoundBox());

        //Resolve the collision
        if (!IsVectorEqual(contactPoint, XCVec4::XCFloat4ZeroVector))
        {
            //This is a impulse particle contact resolver, it actually first puts an object on a vertex
            XCVec4 direction = XCVec4(0.0f, (f32)(contactPoint.Get<Y>()) + (f32)0.1, 0.0f, 0.0f);
            m_particleContact.ApplyImpulse(bboxActor, direction);

            //This I am trying now to make it work with existing collision resolver, works perfectly.
            XCVec4 up = XCVec4(0.0f, 1.0f, 0.0f, 0.0f);
            m_particleContact.ContactResolve(bboxActor, terrainActor, 1.0f, 0.2f, up);

            //Logger("[Terrain Collision] Hit with %s with contact point %f \n", bboxActor->getMesh()->getUserFriendlyName().c_str(), XMVectorGetY(contactPoint));
        }
        return true;

        break;
    }

    case COLLISIONDETECTIONTYPE_BULLET | PhysicsBoundType_OBB:
    {
        IPhysicsFeature* objectActor = obj1->GetCollisionDetectionType() == PhysicsBoundType_OBB ? obj1 : obj2;
        IPhysicsFeature* bulletActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_BULLET ? obj1 : obj2;

        DirectX::ContainmentType type = objectActor->GetBoundBox()->m_TransformedBox.Contains(bulletActor->GetBoundBox()->m_TransformedBox);

        if (type == DirectX::CONTAINS || type == DirectX::INTERSECTS)
        {
            //Before resolving know which corner point was hit
            DirectX::XMFLOAT3 points[8];
            obj1->GetBoundBox()->m_TransformedBox.GetCorners(points);

            XCVec4 contactNormal;

            contactNormal = CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(obj1->GetBoundBox(), obj2->GetBoundBox());

            //Resolve the collision
            m_particleContact.ContactResolve(obj1, obj2, 1.0f, 0.5f, contactNormal);

            objectActor->Invalidate();
            bulletActor->Invalidate();

            Logger("[BULLET HIT] Hit Obj1 : %d Obj 2: %d", obj1->GetCollisionDetectionType(), obj2->GetCollisionDetectionType());

            return true;
        }
    }

    default:
        break;
    }
    */
    return false;
}