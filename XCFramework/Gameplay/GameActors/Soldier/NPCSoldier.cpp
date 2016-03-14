/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "NPCSoldier.h"
#include "Gameplay/World.h"

NPCSoldier::NPCSoldier(void)
{
}

NPCSoldier::~NPCSoldier(void)
{
}

void NPCSoldier::Init(int actorId)
{
    Soldier::Init(actorId);

    World& world = (World&)SystemLocator::GetInstance()->RequestSystem("World");

    //Initialize the AIBrain
    m_AINavigator = std::make_unique<AINavigator>(this);

    m_AIBrain = std::make_unique<AIBrain>(world, m_AINavigator.get());

    //This is a hack, brain needs to decide from the sensors.
    m_AIBrain->SetState(ACTIONSTATE_WALK);
}

void NPCSoldier::PreLoad(const void* fbBuffer)
{
    const FBSoldier* soldierBuff = (FBSoldier*)fbBuffer;
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    Soldier::PreLoad(XCVec3(soldierBuff->Position()->x(), soldierBuff->Position()->y(), soldierBuff->Position()->z()), (XCMesh*)resMgr.GetResource(soldierBuff->XCMeshResourceName()->c_str()));

    PhysicsActor::PreLoad(fbBuffer);
}

void NPCSoldier::Update(float dt)
{
    //Update Brain with current scenarios.
    m_AIBrain->Update(dt);
    m_AINavigator->Update(dt);

    Integrator(dt);
    ClearForce();

    XCVec3 pos;
    XMStoreFloat3(&pos, m_Position);

#if defined(WIN32)
    m_MTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);
#elif defined(XC_ORBIS)
    m_MTranslation = XMMatrixTranslation(pos.getX(), pos.getY(), pos.getZ());
#endif

    m_currentPosition = m_Position;

    Soldier::Update(dt);
}

void NPCSoldier::Destroy()
{
    Soldier::Destroy();
}