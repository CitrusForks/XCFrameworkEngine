/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "IPhysicsFeature.h"

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"

class PhysicsPlayground;
class ParticleContact;
class PhysicsBoundVolumeGenerator;

class PhysicsCollisionResolverTask : public AsyncTask
{
public:
    PhysicsCollisionResolverTask(PhysicsPlayground& phyPlayground)
        : AsyncTask("PhysicsCollisionTask")
        , m_physicsPlayground(phyPlayground)
    {}
    ~PhysicsCollisionResolverTask() {}

    void                              Init();
    void                              Run();
    void                              Destroy();

private:
    PhysicsPlayground&                m_physicsPlayground;
};


class PhysicsPlayground : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(PhysicsPlayground)

    struct PhysicsPlaygroundDesc
    {
        PhysicsPlaygroundDesc(bool enableCollision, bool threadedCollisionDetection, XCVec4& gravity)
            : m_gravity(gravity)
            , m_enableCollision(enableCollision)
            , m_threadedCollisionTest(threadedCollisionDetection)
        {}

        XCVec4      m_gravity;
        bool        m_enableCollision;
        bool        m_threadedCollisionTest;
    };

    PhysicsPlayground();
    ~PhysicsPlayground();

    void                              Init(PhysicsPlaygroundDesc& playgroundDesc, TaskManager& m_taskManager);
    void                              Update(float dt);
    void                              Destroy();
    
    IPhysicsFeature*                  CreatePhysicsFeature(const PhysicsDesc& phydesc);
    void                              DestroyPhysicsFeature(IPhysicsFeature* phyFeature);

    bool                              IsGravityEnabled() const { return m_gravityEnabled; }
    void                              SetGravity(bool enable) { m_gravityEnabled = enable; }

    void                              SetAcceleratedGravity(const XCVec4& gravity) { m_GAcceleration = gravity; }
    XCVec4                            GetAcceleratedGravity() const { return m_GAcceleration; }

    void                              SetCollisionDetection(bool enable) { m_enableCollisionDetection = enable; }
    void                              TestCollision();

protected:
    bool                              CheckCollision(IPhysicsFeature* obj1, IPhysicsFeature* obj2);

private:
    std::vector<IPhysicsFeature*>     m_physicsFeatures;
    PhysicsCollisionResolverTask*     m_collisionResolverTask;
    TaskManager*                      m_taskManager;
    ParticleContact*                  m_particleContact;
    PhysicsBoundVolumeGenerator*      m_boundVolumeGenerator;

    XCVec4                            m_GAcceleration;
    bool                              m_enableCollisionDetection;
    bool                              m_threadedCollisionTest;
    bool                              m_gravityEnabled;
};