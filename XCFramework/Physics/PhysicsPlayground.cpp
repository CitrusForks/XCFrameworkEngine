/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */


#include "EnginePrecompiledHeader.h"

#include "PhysicsPlayground.h"
#include "CollisionDetection.h"

//TODO: Remove this thing
#include "Gameplay/GameActors/Environment/Terrain/Terrain.h"

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
    //Initialize the collision thread
    m_collisionResolverTask = XCNEW(PhysicsCollisionResolverTask)(*this);
    m_collisionResolverTask->SetTaskPriority(THREAD_PRIORITY_HIGHEST);
    m_taskManager->RegisterTask(m_collisionResolverTask);
}

void PhysicsPlayground::Destroy()
{
    m_collisionResolverTask->Destroy();
    m_taskManager->UnregisterTask(m_collisionResolverTask->GetThreadId());
    XCDELETE(m_collisionResolverTask);
}

void PhysicsPlayground::AddPhysicsActor(const PhysicsActor& phyActor)
{

}

void PhysicsPlayground::RemovePhysicsActor(const PhysicsActor& phyActor)
{

}

void PhysicsPlayground::TestCollision()
{
    if (m_physicsObjects.size() > 0)
    {
        std::vector<PhysicsActor*>::iterator obj1;
        for (obj1 = m_physicsObjects.begin(); obj1 != m_physicsObjects.end() - 1; ++obj1)
        {
            std::vector<PhysicsActor*>::iterator obj2;
            for (obj2 = obj1 + 1; obj2 != m_physicsObjects.end(); ++obj2)
            {
                if (*obj1 && *obj2 && CheckCollision(*obj1, *obj2))
                {
                    //Logger("COLLIDED VERY WELL");
                }
            }
        }
    }
}

bool PhysicsPlayground::CheckCollision(PhysicsActor* obj1, PhysicsActor* obj2)
{
    switch (obj1->GetCollisionDetectionType() | obj2->GetCollisionDetectionType())
    {
    case COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
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

    case COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX | COLLISIONDETECTIONTYPE_TRIANGLE:

        break;

    case COLLISIONDETECTIONTYPE_TERRAIN | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
    {
        //Handle terrain and boundbox collision
        PhysicsActor* bboxActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX ? obj1 : obj2;
        PhysicsActor* terrainActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_TERRAIN ? obj1 : obj2;

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

    case COLLISIONDETECTIONTYPE_BULLET | COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX:
    {
        PhysicsActor* objectActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX ? obj1 : obj2;
        PhysicsActor* bulletActor = obj1->GetCollisionDetectionType() == COLLISIONDETECTIONTYPE_BULLET ? obj1 : obj2;

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

    return false;
}