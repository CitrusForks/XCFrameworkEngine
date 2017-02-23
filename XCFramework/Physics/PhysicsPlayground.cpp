/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */


#include "PhysicsPrecompiledHeader.h"

#include "PhysicsPlayground.h"
#include "PhysicsDesc.h"
#include "CollisionDetection.h"
#include "ParticleContact.h"

#include "PhysicsBoundVolumeGenerator.h"
#include "IPhysicsBoundVolume.h"

#include "Phusike/BoundVolumeGenerator.h"
#include "Phusike/RigidBody.h"
#include "Phusike/StaticBody.h"
#include "Phusike/OBBBoundVolume.h"
#include "Phusike/HeightfieldBoundVolume.h"

//WorldCollisionTask--------------------------------------
void PhysicsCollisionResolverTask::Init()
{
    AsyncTask::Init();
}

void PhysicsCollisionResolverTask::Run()
{
    AsyncTask::Run();

    m_physicsPlayground.TestCollision();
}

void PhysicsCollisionResolverTask::Destroy()
{
    AsyncTask::Destroy();
}


//PhysicsPlayground----------------------------------------
PhysicsPlayground::PhysicsPlayground()
    : m_collisionResolverTask(nullptr)
    , m_taskManager(nullptr)
    , m_particleContact(nullptr)
    , m_boundVolumeGenerator(nullptr)
    , m_enableCollisionDetection(false)
    , m_threadedCollisionTest(false)
    , m_gravityEnabled(false)
{
}

PhysicsPlayground::~PhysicsPlayground()
{
}

void PhysicsPlayground::Init(PhysicsPlaygroundDesc& playgroundDesc, TaskManager& taskMgr)
{
    m_addRemoveLock.Create();

    m_taskManager = &taskMgr;

    m_GAcceleration = playgroundDesc.m_gravity;
    m_enableCollisionDetection = playgroundDesc.m_enableCollision;
    m_threadedCollisionTest = playgroundDesc.m_threadedCollisionTest;

    //Initialize the particle contact resolver and the bound volume generator
    m_particleContact = XCNEW(ParticleContact)();
    m_boundVolumeGenerator = XCNEW(BoundVolumeGenerator)();

    if(playgroundDesc.m_threadedCollisionTest)
    {
        //Initialize the collision thread
        m_collisionResolverTask = XCNEW(PhysicsCollisionResolverTask)(*this);
        m_collisionResolverTask->SetTaskPriority(THREAD_PRIORITY_HIGHEST);
        m_taskManager->RegisterTask(m_collisionResolverTask);
    }
}

void PhysicsPlayground::Update(float dt)
{
    for (auto& phyFeature : m_physicsFeatures)
    {
        phyFeature->Update(dt);
    }

    if(m_enableCollisionDetection)
    {
        if(!m_threadedCollisionTest)
        {
            TestCollision();
        }
        else
        {
            //Signal the collision resolver thread.
        }
    }
}

void PhysicsPlayground::Destroy()
{
    if(m_threadedCollisionTest)
    {
        m_collisionResolverTask->Destroy();
        m_taskManager->UnregisterTask(m_collisionResolverTask->GetThreadId());
        XCDELETE(m_collisionResolverTask);
    }

    m_addRemoveLock.Release();

    XCDELETE(m_boundVolumeGenerator);
    XCDELETE(m_particleContact);
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

    //Create the bound volume and assign it to the feature.
    IPhysicsBoundVolume* boundVolume = m_boundVolumeGenerator->GenerateBoundVolume(phydesc.m_boundVolumeDesc);
    feature->SetBoundType(boundVolume);

    //Initialize the feature
    feature->Init(phydesc);

    m_addRemoveLock.Enter();
    m_physicsFeatures.push_back(feature);
    m_addRemoveLock.Exit();

    return feature;
}

void PhysicsPlayground::DestroyPhysicsFeature(IPhysicsFeature* phyActor)
{
    m_addRemoveLock.Enter();

    auto& feature = std::find_if(m_physicsFeatures.begin(), m_physicsFeatures.end(), 
        [phyActor](const IPhysicsFeature* obj) -> bool
    {
        return obj == phyActor;
    });

    if (feature != m_physicsFeatures.end())
    {
        XCDELETE(*feature);
        m_physicsFeatures.erase(feature);
    }
    else
    {
        XCASSERT(false);
    }

    m_addRemoveLock.Exit();
}

void PhysicsPlayground::TestCollision()
{
    if (m_physicsFeatures.size() > 0)
    {
        m_addRemoveLock.Enter();

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

        m_addRemoveLock.Exit();
        
        Thread::ThreadSleep(1);
    }
}

bool PhysicsPlayground::CheckCollision(IPhysicsFeature* obj1, IPhysicsFeature* obj2)
{
    u32 mask = obj1->GetBoundType() | obj2->GetBoundType();
    switch (mask)
    {
    case PhysicsBoundType_Box | PhysicsBoundType_Box:
        {
            const OBBBoundVolume* obb1 = obj1->GetBoundVolume()->GetTyped<OBBBoundVolume>();
            const OBBBoundVolume* obb2 = obj2->GetBoundVolume()->GetTyped<OBBBoundVolume>();

            if(obb1->Intersects(obb2))
            {
                //Before resolving know which corner point was hit
                DirectX::XMFLOAT3 points[8];
                obb1->GetTransformedBox().GetCorners(points);

                XCVec4 contactNormal;

                contactNormal = CollisionDetection::GetContactNormalFromOBBToOBBTriangleTest(obb1, obb2);
                //contactNormal = getContactNormalFromBoundBoxContainedPlane(obj2->getMesh()->getAABoundBox(), obj1->getMesh()->getAABoundBox());
                //Resolve the collision
                m_particleContact->ContactResolve(obj1, obj2, 1.0f, 0.0f, contactNormal);
                return true;
            }
            break;
        }

    case PhysicsBoundType_HeightField | PhysicsBoundType_Box:
        {
            // Handle terrain and boundbox collision
            IPhysicsFeature* bboxActor = nullptr;
            IPhysicsFeature* terrainActor = nullptr;

            if(obj1->GetBoundType() == PhysicsBoundType_Box)
            {
                bboxActor = obj1;
                terrainActor = obj2;
            }
            else
            {
                terrainActor = obj1;
                bboxActor = obj2;
            }

            XCVec4 contactPoint = CollisionDetection::GetHeightfieldPointOfContactWithBoundBox(
                bboxActor->GetBoundVolume()->GetTyped<IPhysicsBoundVolume>(),
                terrainActor->GetBoundVolume()->GetTyped<IPhysicsBoundVolume>());

            // Resolve the collision
            if(!IsVectorEqual(contactPoint, XCVec4::XCFloat4ZeroVector))
            {
                // This is a impulse particle contact resolver, it actually first puts an object on a vertex
                XCVec4 direction = XCVec4(0.0f, (f32) (contactPoint.Get<Y>()) + (f32)0.1, 0.0f, 0.0f);
                m_particleContact->ApplyImpulse(bboxActor, direction);

                // This I am trying now to make it work with existing collision resolver, works perfectly.
                XCVec4 up = XCVec4(0.0f, 1.0f, 0.0f, 0.0f);
                m_particleContact->ContactResolve(bboxActor, terrainActor, 1.0f, 0.2f, up);

                // Logger("[Terrain Collision] Hit with %s with contact point %f \n", bboxActor->getMesh()->getUserFriendlyName().c_str(),
                // XMVectorGetY(contactPoint));
            }
            return true;

            break;
        }
    /*
    case COLLISIONDETECTIONTYPE_BULLET | PhysicsBoundType_OBB:
    {
        IPhysicsFeature* objectActor = obj1->GetBoundType() == PhysicsBoundType_OBB ? obj1 : obj2;
        IPhysicsFeature* bulletActor = obj1->GetBoundType() == COLLISIONDETECTIONTYPE_BULLET ? obj1 : obj2;

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

            Logger("[BULLET HIT] Hit Obj1 : %d Obj 2: %d", obj1->GetBoundType(), obj2->GetBoundType());

            return true;
        }
    }
    */
    default:
        break;
    }

    return false;
}

#if defined(DEBUG_PHYSICS_OBB)
void PhysicsPlayground::GetOBBInfo(std::vector<OBBInfo>& outInfo)
{
    for(auto& physicsFeature : m_physicsFeatures)
    {
        physicsFeature->GetBoundVolume()->GetOBBInfo(outInfo);
    }
}
#endif