/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Bullet.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Gameplay/GameActors/Weapons/Bullets/BulletsManager.h"
#include "Gameplay/GameStates/RunningState.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"

Bullet::Bullet(void)
{
}

Bullet::~Bullet(void)
{
}

void Bullet::PreLoad(XCVec3 initialPosition, XCVec3 target, XCMesh* pMesh)
{
    m_pMesh = pMesh;

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_texture = (Texture2D*)resMgr.GetResource("pistol_bullet_mesh_tex");

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);

    m_useShaderType = SHADERTYPE_LIGHTTEXTURE;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_BULLET;

    //Assign the look
    m_target = XMLoadFloat3(&target);

    Logger("[Bullet] Preload done");

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectBuffer)));
#endif
}

void Bullet::Load()
{
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MInitialRotation = XMMatrixRotationY(XM_PIDIV2);
    m_MInitialRotation *= XMMatrixRotationZ(XM_PIDIV2);

    m_transformedRotation = m_MRotation;
    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    BuildMeshBuffer();

    //Trigger the bullet or add force to move
    m_look = m_target;

    PhysicsActor::Load();

    //Load done. Shoot the bullet
    Shoot(5000.0f);
    Logger("[Bullet] Load done");
}

void Bullet::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XMLoadFloat3(&vec), XMLoadFloat3(&vec), 1, (float)0.2);
}

void Bullet::Shoot(float scalarForce)
{
    AddForce(m_look * scalarForce);
}

void Bullet::BuildMeshBuffer()
{
    m_pMesh->CreateBuffers(VertexFormat_PositionNormalTexture);
}

void Bullet::Update(float dt)
{
    PhysicsActor::Update(dt);

    Integrator(dt);

    XCVec3 pos;
    XMStoreFloat3(&pos, m_Position);

#if defined(WIN32)
    m_MTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);
#elif defined(XC_ORBIS)
    m_MTranslation = XMMatrixTranslation(pos.getX(), pos.getY(), pos.getZ());
#endif

    m_currentPosition = m_Position;

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    PhysicsActor::Update(dt);

    m_pMesh->Update(dt);

    if (XMVectorGetY(m_currentPosition) < -10.0f)
    {
        Invalidate();
    }
}

void Bullet::Draw(RenderContext& context)
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

    PhysicsActor::Draw(context);
}

void Bullet::Destroy()
{
    PhysicsActor::Destroy();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBPerObject);
#endif
}