/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Car.h"

#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCMesh/XCMesh.h"

#include "Gameplay/XCCamera/XCCameraManager.h"

#include "Engine/Resource/ResourceManager.h"

#include "Physics/PhysicsPlayground.h"
#include "Physics/Phusike/RigidBody.h"

Car::Car(void)
{
}

Car::~Car(void)
{
}

IActor::ActorReturnState Car::LoadMetaData( const void* metaData )
{
    const FBCar* carBuff = static_cast<const FBCar*>(metaData);
    PhysicsActor::LoadMetaData(carBuff->Base());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(carBuff->XCMeshResourceName()->c_str());

    m_useShaderType = ShaderType_LightTexture;

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState Car::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition);
    
    m_MRotation= MatrixRotationXYZ(m_initialRotation);
    
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    return PhysicsActor::Load();
}

void Car::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();

    PhysicsPlayground& playground = SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");
    PhysicsDesc desc(PhysicsBodyType_RigidDynamic,
                     PhysicsBoundType_Box,
                     m_currentPosition,
                     500,
                     0.8);
    m_pMesh->GetResource<XCMesh>()->GetBounds(desc.m_boundVolumeDesc.m_boundDesc.m_boundBoxDesc.m_min, desc.m_boundVolumeDesc.m_boundDesc.m_boundBoxDesc.m_max);
    m_physicsFeature = playground.CreatePhysicsFeature(desc);
}

IActor::ActorReturnState Car::Update(f32 dt)
{
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    m_pMesh->GetResource<XCMesh>()->Update(dt);
    
    return PhysicsActor::Update(dt);
}

void Car::Accelerate(f32 distance)
{
    m_currentPosition += (distance * m_look);
}

void Car::Steer(f32 angle, f32 scalarForce)
{
    XCVec4 quaternionAxis = QuaternionRotationAxis(XCVec4(0, 1, 0, 0), angle);
    XCMatrix4 rotation = MatrixRotationQuaternion(quaternionAxis);

    m_look = VectorTransformNormalMatrix(m_look, rotation);
    m_right = VectorTransformNormalMatrix(m_right, rotation);

    //Rotate the car with it's initial rotations.
    m_MRotation *= rotation;
    XCVec4 rotvec = QuaternionRotationMatrix(m_MRotation);

    m_physicsFeature->GetTyped<RigidBody>()->AddRotationalForce(rotvec);
    m_physicsFeature->GetTyped<RigidBody>()->AddForce(m_look * scalarForce);
}

bool Car::Draw(RenderContext& renderContext)
{
    // Set constants
    ICamera& cam = renderContext.GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
       MatrixTranspose(m_World).GetUnaligned(),
       MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
       MatrixInverseTranspose(m_World).GetUnaligned(),
       XCMatrix::XCMatrixIdentity.GetUnaligned(),
        m_material
    };

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    return PhysicsActor::Draw(renderContext);
}

IActor::ActorReturnState Car::Destroy()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);

    return PhysicsActor::Destroy();
}