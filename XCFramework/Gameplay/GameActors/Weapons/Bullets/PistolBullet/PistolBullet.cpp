/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "PistolBullet.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

PistolBullet::PistolBullet(void)
{
}

PistolBullet::~PistolBullet(void)
{
}

PistolBullet::PistolBullet(IActor* parentActor, XCVec3 initialPosition, XCMesh* pMesh)
{
    m_pMesh = pMesh;

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);


    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);

    m_useShaderType = SHADERTYPE_LIGHTTEXTURE;

    m_secondaryLookAxis = XMVectorZero();
    m_secondaryUpAxis = XMVectorZero();
    m_secondaryRightAxis = XMVectorZero();
}

void PistolBullet::Init(int actorId)
{
    m_currentPosition += GetOffsetPosition();

    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MInitialRotation = XMMatrixRotationY(XM_PIDIV2);
    m_MInitialRotation *= XMMatrixRotationZ(XM_PIDIV2);

    m_transformedRotation = m_MRotation;
    m_MRotation = m_transformedRotation * m_MInitialRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildGeometryBuffer();

    m_secondaryLookAxis = m_look;
    m_secondaryUpAxis = m_up;
    m_secondaryRightAxis = m_right;

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer)));
#endif
}

void PistolBullet::BuildGeometryBuffer()
{
    m_pMesh->CreateBuffers(VertexFormat_PositionNormalTexture);
}


void PistolBullet::Update(float dt)
{
    UpdateOffsets(dt);

    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->Update(dt);

    SimpleMeshActor::Update(dt);
}

void PistolBullet::UpdateOffsets(float dt)
{
    m_look = m_bindedActor->GetLook();
    m_right = m_bindedActor->GetRight();
    m_up = m_bindedActor->GetUp();

    m_secondaryLookAxis = GetOffsetLook();
    m_secondaryUpAxis = GetOffsetUp();
    m_secondaryRightAxis = GetOffsetRight();

    m_currentPosition = m_bindedActor->GetPosition();

    m_currentPosition += (XMVectorGetX(m_offsetPosition) * m_right);
    m_currentPosition += (XMVectorGetY(m_offsetPosition) * m_secondaryUpAxis);
    m_currentPosition += (XMVectorGetZ(m_offsetPosition) * m_secondaryLookAxis);

    //m_transformedRotation *= GetOffsetRotation();
}

void PistolBullet::ApplyOffsetRotation()
{
    m_transformedRotation *= GetOffsetRotation();
}

void PistolBullet::Draw(RenderContext& context)
{
    context.SetRasterizerState(RASTERIZERTYPE_FILL_SOLID);
    context.ApplyShader(m_useShaderType);
#if defined(XCGRAPHICS_DX11)
    context.GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#endif
    unsigned int stride = sizeof(VertexPosNormTex);
    unsigned int offset = 0;
    
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

    SimpleMeshActor::Draw(context);
}

void PistolBullet::Destroy()
{
    SimpleMeshActor::Destroy();
}