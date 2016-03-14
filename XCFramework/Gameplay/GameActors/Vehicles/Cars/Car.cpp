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

Car::Car(void)
{
}

Car::~Car(void)
{
}

void Car::PreLoad(XCVec3 initialPosition, XCMesh* pMesh)
{
    m_pMesh = pMesh;
    
    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);
    
    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);
    
    m_useShaderType = SHADERTYPE_LIGHTTEXTURE;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer)));
#endif
}

void Car::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));
    
    m_MRotation= XMMatrixRotationX(-XM_PIDIV2);
    
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    BuildMeshBuffer();
    
    PhysicsActor::Load();
}

void Car::SetInitialPhysicsProperties()
{
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XMLoadFloat3(&vec), XMLoadFloat3(&vec), 10, (float)0.8);
    PhysicsActor::SetInitialPhysicsProperties();
}

void Car::BuildMeshBuffer()
{
    m_pMesh->createBuffers(VertexFormat_PositionNormalTexture);
}

void Car::Update(float dt)
{
    m_World = m_MScaling * m_MRotation * m_MTranslation;
    
    m_pMesh->Update(dt);
    
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
    context.SetRasterizerState(RASTERIZERTYPE_FILL_SOLID);
    context.ApplyShader(m_useShaderType);

    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    cbPerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    XCShaderHandle* lightTexShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_useShaderType);
#if defined(XCGRAPHICS_DX12)
    memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(cbPerObjectBuffer));
    lightTexShader->setConstantBuffer("cbPerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#else
    lightTexShader->setCBPerObject(context.GetDeviceContext(), perObject);
#endif

    m_pMesh->Draw(context, m_useShaderType);

    PhysicsActor::Draw(context);
}

void Car::Destroy()
{
    PhysicsActor::Destroy();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBPerObject);
#endif
}