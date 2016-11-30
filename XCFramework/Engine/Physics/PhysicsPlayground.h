/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Actors/PhysicsActor.h"
#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"

#include "ParticleContact.h"

class PhysicsPlayground;

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


class PhysicsPlayground
{
public:

    void                              Init(TaskManager& m_taskManager);
    void                              Destroy();
    void                              AddPhysicsActor(const PhysicsActor& phyActor);
    void                              RemovePhysicsActor(const PhysicsActor& phyActor);
    void                              TestCollision();

protected:
    bool                              CheckCollision(PhysicsActor* obj1, PhysicsActor* obj2);

private:
    std::vector<PhysicsActor*>        m_physicsObjects;
    PhysicsCollisionResolverTask*     m_collisionResolverTask;
    TaskManager*                      m_taskManager;
    ParticleContact                   m_particleContact;
};