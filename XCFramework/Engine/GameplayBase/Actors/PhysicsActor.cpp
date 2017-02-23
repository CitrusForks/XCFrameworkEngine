/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "PhysicsActor.h"

#include "Engine/Resource/ResourceHandle.h"

#include "Graphics/XCMesh/XCMesh.h"

#include "Physics/IPhysicsFeature.h"
#include "Physics/Phusike/RigidBody.h"

PhysicsActor::PhysicsActor()
    : m_physicsFeature(nullptr)
    , m_pMesh(nullptr)
{
}

PhysicsActor::~PhysicsActor(void)
{
}

IActor::IActor::ActorReturnState PhysicsActor::Init()
{
    IActor::IActor::ActorReturnState result = AnimatedActor::Init();

    m_look  = XCVec4(0, 0, 1, 0);
    m_up    = XCVec4(0, 1, 0, 0);
    m_right = VectorCross(m_up, m_look);

    m_transformedRotation = m_MRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

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
}

void PhysicsActor::AddForce(const XCVec4& force)
{
    m_physicsFeature->GetTyped<RigidBody>()->AddForce(force);
}

void PhysicsActor::AddRotationalForce(const XCVec4& force)
{
    m_physicsFeature->GetTyped<RigidBody>()->AddRotationalForce(force);
}

IActor::IActor::ActorReturnState PhysicsActor::Update(f32 dt)
{
    return IActor::Update(dt);
}

bool PhysicsActor::Draw(RenderContext& renderContext)
{
    return IActor::Draw(renderContext);
}

IActor::IActor::ActorReturnState PhysicsActor::Unload()
{
    return IActor::Unload();
}

IActor::IActor::ActorReturnState PhysicsActor::Destroy()
{
    XCDELETE(m_physicsFeature);

    return IActor::Destroy();
}