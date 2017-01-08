/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

#include "IPhysicsFeature.h"

#include "Engine/TaskManager/Task/AsyncTask.h"
#include "Engine/TaskManager/TaskManager.h"

#include "ParticleContact.h"

class RenderContext;
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


class PhysicsPlayground : public ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(PhysicsPlayground)

    void                              Init(TaskManager& m_taskManager);
    void                              Update(float dt);
    void                              Draw(RenderContext& context);
    void                              Destroy();
    
    IPhysicsFeature*                  CreatePhysicsFeature(const PhysicsDesc& phydesc);
    void                              RemovePhysicsFeature(IPhysicsFeature* phyFeature);
    
    void                              TestCollision();
    XCVec4                            GetAcceleratedGravity() const { return m_GAcceleration; }

protected:
    bool                              CheckCollision(IPhysicsFeature* obj1, IPhysicsFeature* obj2);

private:
    std::vector<IPhysicsFeature*>     m_physicsFeatures;
    PhysicsCollisionResolverTask*     m_collisionResolverTask;
    TaskManager*                      m_taskManager;
    ParticleContact                   m_particleContact;

    XCVec4                            m_GAcceleration;
};