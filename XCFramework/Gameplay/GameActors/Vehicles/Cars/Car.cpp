/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Car.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Resource/ResourceManager.h"

Car::Car(void)
{
}

Car::~Car(void)
{
}

void Car::PreLoad(XCVec3 initialPosition, std::string pMesh)
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(pMesh.c_str());
    
    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);
    
    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);
    
    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;
}

void Car::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));
    
    m_MRotation= XMMatrixRotationX(-XM_PIDIV2);
    
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    PhysicsActor::Load();
}

void Car::SetInitialPhysicsProperties()
{
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XMLoadFloat3(&vec), XMLoadFloat3(&vec), 10, (float)0.8);
    PhysicsActor::SetInitialPhysicsProperties();
}

void Car::Update(float dt)
{
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    m_pMesh->GetResource<XCMesh>()->Update(dt);
    
    PhysicsActor::Update(dt);
}

void Car::Accelerate(float distance)
{
    m_Position += (distance * m_look);
}

void Car::Steer(float angle, float scalarForce)
{
    XCVecIntrinsic4 quaternionAxis = XMQuaternionRotationAxis(toXMVECTOR(0, 1, 0, 0), angle);
    XCMatrix4 rotation = XMMatrixRotationQuaternion(quaternionAxis);

    m_look = XMVector3TransformNormal(m_look, rotation);
    m_right = XMVector3TransformNormal(m_right, rotation);

    //Rotate the car with it's initial rotations.
    m_MRotation *= rotation;

    AddForce(m_look * scalarForce);
}

void Car::Draw(RenderContext& context)
{
    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    PerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    PhysicsActor::Draw(context);
}

void Car::Destroy()
{
    PhysicsActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);
}