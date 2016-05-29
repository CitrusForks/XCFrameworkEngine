/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gameplay/GameActors/PhysicsActor.h"
#include "Gameplay/World.h"

PhysicsActor::PhysicsActor()
{
    m_pMesh = nullptr;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_MAX;
    m_actorType = GAMEACTOR_PHYSICS;

    m_boundBox = new RenderableOBB();
}

PhysicsActor::~PhysicsActor(void)
{
    delete(m_boundBox);
}

void PhysicsActor::Init(int actorId)
{
    IActor::Init(actorId);

    XCVec3 look = XCVec3(0, 0, 1);
    m_look = XMLoadFloat3(&look);
    XCVec3 up = XCVec3(0, 1, 0);
    m_up = XMLoadFloat3(&up);
    m_right = XMVector3Cross(m_up, m_look);

    m_MTranslation = XMMatrixIdentity();
    m_MRotation = XMMatrixIdentity();
    m_MScaling = XMMatrixIdentity();
    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_boundBox->Init();
}

void PhysicsActor::Load()
{
    AnimatedActor::Load();
}

void PhysicsActor::UpdateState()
{
    if (m_pMesh && m_pMesh->GetResource<XCMesh>()->IsLoaded())
    {
        SetInitialPhysicsProperties();
        m_actorState = IActor::ActorState_Loaded;
    }
    else if (m_pMesh == nullptr)
    {
        //We do not have a mesh. SO its loaded
        m_actorState = IActor::ActorState_Loaded;
    }

    IActor::UpdateState();
}

void PhysicsActor::SetInitialPhysicsProperties()
{
    if (m_pMesh)
    {
        m_boundBox->CreateBoundBox(m_pMesh->GetResource<XCMesh>()->GetAABB());
        m_boundBox->Transform(m_MTranslation, m_MRotation);
    }
}

void PhysicsActor::Update(float dt)
{
    IActor::Update(dt);

    if (m_pMesh)
    {
        m_boundBox->Transform(m_MTranslation, m_MRotation);
        m_boundBox->Update(dt);
    }
}

void PhysicsActor::Draw(RenderContext& context)
{
    IActor::Draw(context);

    if (m_pMesh)
    {
        m_boundBox->Draw(context);
    }
}

void PhysicsActor::Unload()
{
    IActor::Unload();
}

void PhysicsActor::Destroy()
{
    if (m_pMesh)
    {
        m_boundBox->Destroy();
    }

    IActor::Destroy();
}
