/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Bullet.h"

#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCMesh/XCMesh.h"

Bullet::Bullet(void)
{
}

Bullet::~Bullet(void)
{
}

void Bullet::PreLoad(XCVec3& initialPosition, XCVec3& target, std::string pMeshName)
{
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_pMesh     = &resMgr.AcquireResource(pMeshName.c_str());

    m_material.Ambient = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    m_currentPosition = initialPosition;

    m_useShaderType = ShaderType_LightTexture;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_BULLET;

    //Assign the look
    m_target = target;

    Logger("[Bullet] Preload done");
    PhysicsActor::PreLoad(nullptr);
}

void Bullet::Load()
{
    m_MTranslation = MatrixTranslate(m_currentPosition.Get<X>(), m_currentPosition.Get<Y>(), m_currentPosition.Get<Z>());

    m_MInitialRotation  = MatrixRotationY(XC_PIDIV2);
    m_MInitialRotation *= MatrixRotationZ(XC_PIDIV2);

    m_transformedRotation = m_MRotation;
    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    //Trigger the bullet or add force to move
    m_look = m_target;

    Logger("[Bullet] Load done");

    PhysicsActor::Load();
}

void Bullet::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    InitXPhysics(m_currentPosition, XCVec4(), XCVec4(), 1, (f32)0.2);

    //Load done. Shoot the bullet
    Shoot(5000.0f);
}

void Bullet::Shoot(f32 scalarForce)
{
    AddForce(m_look * scalarForce);
}

void Bullet::Update(f32 dt)
{
    PhysicsActor::Update(dt);

    Integrator(dt);

    m_MTranslation = MatrixTranslate(m_Position.Get<X>(), m_Position.Get<Y>(), m_Position.Get<Z>());

    m_currentPosition = m_Position;

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    PhysicsActor::Update(dt);

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    if (m_currentPosition.Get<Y>() < -10.0f)
    {
        Invalidate();
    }
}

void Bullet::Draw(RenderContext& context)
{
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
        XCMatrix4().GetUnaligned(),
        m_material
    };

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    PhysicsActor::Draw(context);
}

void Bullet::Destroy()
{
    PhysicsActor::Destroy();
}