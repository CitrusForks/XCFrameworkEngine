/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "PistolBullet.h"

#include "Engine/Resource/ResourceManager.h"

#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"

#include "Gameplay/XC_Camera/XC_CameraManager.h"

PistolBullet::PistolBullet(void)
{
}

PistolBullet::~PistolBullet(void)
{
}

PistolBullet::PistolBullet(IActor* parentActor, XCVec3& initialPosition, std::string pMesh)
{
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(pMesh.c_str());

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);


    //Get initial position
    m_currentPosition = initialPosition;

    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;

    m_secondaryLookAxis  = XCFloat4::XCFloat4ZeroVector;
    m_secondaryUpAxis    = XCFloat4::XCFloat4ZeroVector;
    m_secondaryRightAxis = XCFloat4::XCFloat4ZeroVector;
}

void PistolBullet::Init(i32 actorId)
{
    m_currentPosition += GetOffsetPosition();

    m_MTranslation = MatrixTranslate(m_currentPosition);

    m_MInitialRotation  = MatrixRotationY(XC_PIDIV2);
    m_MInitialRotation *= MatrixRotationZ(XC_PIDIV2);

    m_transformedRotation = m_MRotation;
    m_MRotation = m_transformedRotation * m_MInitialRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_secondaryLookAxis = m_look;
    m_secondaryUpAxis = m_up;
    m_secondaryRightAxis = m_right;
}

void PistolBullet::Update(f32 dt)
{
    UpdateOffsets(dt);

    m_MTranslation = MatrixTranslate(m_currentPosition);

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    SimpleMeshActor::Update(dt);
}

void PistolBullet::UpdateOffsets(f32 dt)
{
    m_look = m_bindedActor->GetLook();
    m_right = m_bindedActor->GetRight();
    m_up = m_bindedActor->GetUp();

    m_secondaryLookAxis = GetOffsetLook();
    m_secondaryUpAxis = GetOffsetUp();
    m_secondaryRightAxis = GetOffsetRight();

    m_currentPosition = m_bindedActor->GetPosition();

    m_currentPosition += (m_offsetPosition.Get<X>() * m_right);
    m_currentPosition += (m_offsetPosition.Get<Y>() * m_secondaryUpAxis);
    m_currentPosition += (m_offsetPosition.Get<Z>() * m_secondaryLookAxis);

    //m_transformedRotation *= GetOffsetRotation();
}

void PistolBullet::ApplyOffsetRotation()
{
    m_transformedRotation *= GetOffsetRotation();
}

void PistolBullet::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillSolid);
    context.ApplyShader(m_useShaderType);
#if defined(XCGRAPHICS_DX11)
    context.GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#endif
    u32 stride = sizeof(VertexPosNormTex);
    u32 offset = 0;
    
    // Set constants
    ICamera& cam = context.GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
        MatrixTranspose(m_World).GetUnaligned(),
        MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
        MatrixInverseTranspose(m_World).GetUnaligned(),
        XCMatrix4::XCMatrixIdentity.GetUnaligned(),
        m_material
    };
  
    //m_pMesh->DrawAllInstanced(perObject);

    SimpleMeshActor::Draw(context);
}

void PistolBullet::Destroy()
{
    SimpleMeshActor::Destroy();
}