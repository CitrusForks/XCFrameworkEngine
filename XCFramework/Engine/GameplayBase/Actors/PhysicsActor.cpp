/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "PhysicsActor.h"

#include "Graphics/XCMesh/XCMesh.h"

PhysicsActor::PhysicsActor()
{
    m_pMesh = nullptr;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_MAX;

    m_boundBox = XCNEW(RenderableOBB)();
}

PhysicsActor::~PhysicsActor(void)
{
    XCDELETE(m_boundBox);
}

IActor::IActor::ActorReturnState PhysicsActor::Init()
{
    IActor::IActor::ActorReturnState result = AnimatedActor::Init();

    m_look = XCVec4(0, 0, 1, 0);
    XCVec3 up(0, 1, 0);
    m_up = XCVec4(0, 1, 0, 0);
    m_right = VectorCross(m_up, m_look);

    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_boundBox->Init();

    return result;
}

IActor::IActor::ActorReturnState PhysicsActor::Load()
{
    return AnimatedActor::Load();
}

IActor::IActor::ActorReturnState PhysicsActor::OnLoaded()
{
    IActor::IActor::ActorReturnState result = AnimatedActor::OnLoaded();

    //Since resources are interleaved. We need to wait for them to be loaded. Further way to improve this is, having callbacks when resource is loaded.
    if (m_pMesh == nullptr || (m_pMesh && m_pMesh->GetResource<XCMesh>()->IsLoaded()))
    {
        SetInitialPhysicsProperties();
        result = IActor::IActor::ActorReturnState_Success;
    }
    else
    {
        result = IActor::IActor::ActorReturnState_Processing;
    }

    return result;
}

void PhysicsActor::SetInitialPhysicsProperties()
{
    if (m_pMesh)
    {
        m_boundBox->CreateBoundBox(m_pMesh->GetResource<XCMesh>()->GetAABB());
        m_boundBox->Transform(m_MTranslation, m_MRotation);
    }
}

IActor::IActor::ActorReturnState PhysicsActor::Update(f32 dt)
{
    IActor::IActor::ActorReturnState result = IActor::Update(dt);

    if (m_pMesh)
    {
        m_boundBox->Transform(m_MTranslation, m_MRotation);
        m_boundBox->Update(dt);
    }

    return result;
}

bool PhysicsActor::Draw(RenderContext& renderContext)
{
    bool result = IActor::Draw(renderContext);

    if (m_pMesh)
    {
        m_boundBox->Draw(renderContext);
    }

    return result;
}

IActor::IActor::ActorReturnState PhysicsActor::Unload()
{
    return IActor::Unload();
}

IActor::IActor::ActorReturnState PhysicsActor::Destroy()
{
    if (m_pMesh)
    {
        m_boundBox->Destroy();
    }

    return IActor::Destroy();
}
