/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "NPCSoldier.h"

#include "Engine/GameplayBase/SceneGraph.h"
#include "Engine/AI/AIBrain.h"
#include "Engine/AI/AINavigator.h"

NPCSoldier::NPCSoldier(void)
{
}

NPCSoldier::~NPCSoldier(void)
{
}

IActor::ActorReturnState NPCSoldier::Init()
{
    Soldier::Init();

    SceneGraph& world = (SceneGraph&)SystemLocator::GetInstance()->RequestSystem("SceneGraph");

    //Initialize the AIBrain
    m_AINavigator = std::make_unique<AINavigator>(this);

    m_AIBrain = std::make_unique<AIBrain>(world, m_AINavigator.get());

    //This is a hack, brain needs to decide from the sensors.
    m_AIBrain->SetState(ACTIONSTATE_WALK);

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState NPCSoldier::LoadMetaData( const void* metaData )
{
    const FBNPCSoldier* soldierBuff = (FBNPCSoldier*)metaData;

    Soldier::LoadMetaData(soldierBuff->Base());

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState NPCSoldier::Update(f32 dt)
{
    //Update Brain with current scenarios.
    m_AIBrain->Update(dt);
    m_AINavigator->Update(dt);

    Integrator(dt);
    ClearForce();

    m_MTranslation = MatrixTranslate(m_Position);

    m_currentPosition = m_Position;

    return Soldier::Update(dt);
}