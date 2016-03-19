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

    m_useShaderType = pMesh->IsSkinnedMesh()? SHADERTYPE_SKINNEDCHARACTER : SHADERTYPE_LIGHTTEXTURE;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;

    m_secondaryLookAxis = XMVectorZero();
    m_secondaryUpAxis = XMVectorZero();
    m_secondaryRightAxis = XMVectorZero();

    m_totalPitchAngle = 0.0f;

    //Gun mesh
    m_gun = ((Gun*) actorFactory.CreateActor("Gun"));
    m_gun->PreLoad(this, initialPosition, (XCMesh*) resMgr.GetResource("Gun"));

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    if (m_useShaderType == SHADERTYPE_LIGHTTEXTURE)
    {
        m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer)));
    }
    else
    {
        m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbSkinnedCharacterBuffer)));
    }
#endif
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

    BuildMeshBuffer();

    PhysicsActor::Load();
}

void Soldier::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XMLoadFloat3(&vec), XMLoadFloat3(&vec), 10, (float)0.8);
}


void Soldier::BuildMeshBuffer()
{
    if(m_pMesh->IsSkinnedMesh())
    {
        m_pMesh->CreateBuffers(VertexFormat_PositionNormalTextureBlendIndexBlendWeight);
    }
    else
    {
        m_pMesh->CreateBuffers(VertexFormat_PositionNormalTexture);
    }
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
    context.SetRasterizerState(RASTERIZERTYPE_FILL_SOLID);
    context.ApplyShader(m_useShaderType);
    
    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    
    if (m_useShaderType == SHADERTYPE_LIGHTTEXTURE)
    {
        cbPerObjectBuffer perObject = {
            ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
            ToXCMatrix4Unaligned(InverseTranspose(m_World)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(XMMatrixIdentity())),
            m_material
        };

        XCShaderHandle* lightTexShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_useShaderType);
#if defined(XCGRAPHICS_DX12)
        memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(cbPerObjectBuffer));
        lightTexShader->setConstantBuffer("cbPerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#else
        lightTexShader->setCBPerObject(context.GetDeviceContext(), perObject);
#endif
    }
    else
    {
        static XCMatrix4Unaligned scaling = XMMatrixScaling(0.01f, 0.01f, 0.01f);
        static XCMatrix4Unaligned rotation = XMMatrixRotationX(XM_PI);

        XCMatrix4Unaligned transform = aiMatrixToMatrix4Unaligned(m_pMesh->GetSceneAnimator()->GetGlobalTransform(m_pMesh->GetSceneNode()->mRootNode));
        transform = scaling * transform;

        cbSkinnedCharacterBuffer perObject = {
            ToXCMatrix4Unaligned(XMMatrixTranspose(transform)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(transform * cam->GetViewMatrix() * cam->GetProjMatrix())),
            ToXCMatrix4Unaligned(InverseTranspose(transform)),
            ToXCMatrix4Unaligned(XMMatrixTranspose(XMMatrixIdentity())),
            m_material
        };

        XCShaderHandle* skinnedCharacterShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_useShaderType);
#if defined(XCGRAPHICS_DX12)
        memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(cbSkinnedCharacterBuffer));
        skinnedCharacterShader->setConstantBuffer("cbSkinnedCharacterBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#endif
    }

    m_pMesh->Draw(context, m_useShaderType);

    PhysicsActor::Draw(context);

    m_gun->Draw(context);
}

void Soldier::Destroy()
{
    PhysicsActor::Destroy();
    m_gun->Destroy();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBPerObject);
#endif
}