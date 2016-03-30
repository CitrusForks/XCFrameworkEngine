/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Soldier.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

#include "Engine/Resource/ResourceManager.h"
#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

const float Soldier::MAX_PITCH_ANGLE = XM_PIDIV4;

Soldier::Soldier(void)
{
}

Soldier::~Soldier(void)
{
}

void Soldier::PreLoad(XCVec3 initialPosition, XCMesh* pMesh)
{
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    GameActorsFactory& actorFactory = (GameActorsFactory&)SystemLocator::GetInstance()->RequestSystem("GameActorsFactory");

    m_pMesh = pMesh;

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);


    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);

    m_useShaderType = pMesh->IsSkinnedMesh()? ShaderType_SkinnedCharacter : ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;

    m_secondaryLookAxis = XMVectorZero();
    m_secondaryUpAxis = XMVectorZero();
    m_secondaryRightAxis = XMVectorZero();

    m_totalPitchAngle = 0.0f;

    //Gun mesh
    m_gun = ((Gun*) actorFactory.CreateActor("Gun"));
    m_gun->PreLoad(this, initialPosition, (XCMesh*) resMgr.GetResource("Gun"));
}


void Soldier::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MRotation = m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_secondaryLookAxis = m_look;
    m_secondaryUpAxis = m_up;
    m_secondaryRightAxis = m_right;

    //Call the sub actors methods
    m_gun->Load();
    XCVec3 pos = XCVec3(1.0f, 3.0f, 2.0f);
    m_gun->GetSubActor()->InitOffsets(XMLoadFloat3(&pos), m_secondaryLookAxis, m_secondaryRightAxis, m_secondaryUpAxis);

    PhysicsActor::Load();
}

void Soldier::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XMLoadFloat3(&vec), XMLoadFloat3(&vec), 10, (float)0.8);
}

void Soldier::Update(float dt)
{
    //Update the rotation based on initial and transformed.
    m_MRotation = m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->Update(dt);

    PhysicsActor::Update(dt);

    m_gun->Update(dt);
}

void Soldier::AccelerateCar(float distance)
{
    m_Position += (distance * m_look);
}

void Soldier::Walk(float scalarForce)
{
    AddForce(m_look * scalarForce);
}

void Soldier::Strafe(float scalarForce)
{
    AddForce(m_right * scalarForce);
}

void Soldier::Jump(float scalarForce)
{
    AddForce(m_up * scalarForce);
}

void Soldier::Yaw(float angle, float scalarForce)
{
    //Total RotationOffset will be applied from Pitch. So remember to always YAW AND PITCH together. ApplyOffsetRotation will be applied from Pitch()
    XCVecIntrinsic4 quaternionAxis = XMQuaternionRotationAxis(toXMVECTOR(0, 1, 0, 0), angle);
    XCMatrix4 rotation = XMMatrixRotationQuaternion(quaternionAxis);

    m_look = XMVector3TransformNormal(m_look, rotation);
    m_right = XMVector3TransformNormal(m_right, rotation);
    m_up = XMVector3TransformNormal(m_up, rotation);

    m_secondaryLookAxis = XMVector3TransformNormal(m_secondaryLookAxis, rotation);
    m_secondaryRightAxis = XMVector3TransformNormal(m_secondaryRightAxis, rotation);
    m_secondaryUpAxis = XMVector3TransformNormal(m_secondaryUpAxis, rotation);

    //Rotate the soldier with it's initial rotations.
    m_transformedRotation *= rotation;

    //Set the offset gun rotation too
    m_gun->SetOffsetLook(m_secondaryLookAxis);
    m_gun->SetOffsetUp(m_secondaryUpAxis);
    m_gun->SetOffsetRight(m_secondaryRightAxis);

    m_gun->SetOffsetRotation(rotation);
}

void Soldier::Pitch(float angle, float scalarForce)
{
    //Total RotationOffset will be applied from Pitch. So remember to always YAW AND PITCH together. ApplyOffsetRotation will be applied from Pitch()
    XCVecIntrinsic4 quaternionAxis = XMQuaternionRotationAxis(m_secondaryRightAxis, angle);
    XCMatrix4 rotation = XMMatrixRotationAxis(m_secondaryRightAxis, angle);

    rotation = XMMatrixRotationQuaternion(quaternionAxis);
    //Compare if the look is within the bound looks
    XCVecIntrinsic4 tempLook = XMVector3TransformNormal(m_secondaryLookAxis, rotation);
    
    //The upper limit and lower limit should be with respect to the parent object look axis, which is constant in the sense without pitch (m_look), 45 degree pitch angle
    XCVecIntrinsic4 lookUB = XMVector3TransformNormal(m_look, XMMatrixRotationAxis(m_secondaryRightAxis, MAX_PITCH_ANGLE / 2));
    XCVecIntrinsic4 lookLB = XMVector3TransformNormal(m_look, XMMatrixRotationAxis(m_secondaryRightAxis, -MAX_PITCH_ANGLE / 2));
    
    if (XMVectorGetY(lookLB) >= XMVectorGetY(tempLook) && XMVectorGetY(lookUB) <= XMVectorGetY(tempLook))
    {
        m_secondaryUpAxis = XMVector3TransformNormal(m_secondaryUpAxis, rotation);
        m_secondaryLookAxis = XMVector3TransformNormal(m_secondaryLookAxis, rotation);
    
        m_gun->SetOffsetLook(m_secondaryLookAxis);
        m_gun->SetOffsetUp(m_secondaryUpAxis);
        m_gun->SetOffsetRight(m_secondaryRightAxis);
    
        //This rotation needs to be set for offsets
        m_gun->SetOffsetRotation(m_gun->GetOffsetRotation() * rotation);
    }

    m_gun->ApplyOffsetRotation();
}

void Soldier::ApplyRotation(XCMatrix4 rotation)
{
    //This method makes sure that the rotation is applied to parent and it's sub actors.
    m_MRotation *= rotation;
    m_gun->SetOffsetRotation(m_gun->GetOffsetRotation() * rotation);
}


void Soldier::Draw(RenderContext& context)
{
    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    PerObjectBuffer perObject = {};

    if (m_useShaderType == ShaderType_LightTexture)
    {
        perObject = {
            ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
            ToXCMatrix4Unaligned(InverseTranspose(m_World)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(XMMatrixIdentity())),
            m_material
        };
    }
    else if(m_useShaderType == ShaderType_SkinnedCharacter)
    {
        static XCMatrix4Unaligned scaling = XMMatrixScaling(1.0f, 1.0f, 1.0f);
        static XCMatrix4Unaligned rotation = XMMatrixRotationX(XM_PI);

        XCMatrix4Unaligned transform = m_pMesh->GetRootTransform();
        //transform = scaling * transform;

        perObject = {
            ToXCMatrix4Unaligned(XMMatrixTranspose(transform)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(transform * cam->GetViewMatrix() * cam->GetProjMatrix())),
            ToXCMatrix4Unaligned(InverseTranspose(transform)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(XMMatrixIdentity())),
            m_material
        };
    }

    m_pMesh->DrawInstanced(perObject);

    PhysicsActor::Draw(context);

    m_gun->Draw(context);
}

void Soldier::Destroy()
{
    PhysicsActor::Destroy();
    m_gun->Destroy();
}