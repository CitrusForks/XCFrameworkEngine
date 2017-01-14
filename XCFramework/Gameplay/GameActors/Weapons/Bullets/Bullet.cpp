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

#include "Physics/PhysicsPlayground.h"
#include "Physics/Phusike/RigidBody.h"

Bullet::Bullet(void)
{
}

Bullet::~Bullet(void)
{
}

IActor::ActorReturnState Bullet::LoadMetaData( const void* metaData )
{
    //TODO : Gen a fb in runtime
    XCASSERT(false);

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    //m_pMesh     = &resMgr.AcquireResource(pMeshName.c_str());

    m_material.Ambient = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    //m_currentPosition = initialPosition;

    m_useShaderType = ShaderType_LightTexture;

    //Assign the look
    //m_target = target;

    Logger("[Bullet] Preload done");
    return PhysicsActor::LoadMetaData(metaData);
}

IActor::ActorReturnState Bullet::Load()
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

    return PhysicsActor::Load();
}

void Bullet::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();

    PhysicsPlayground& playground = SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");
    PhysicsDesc desc(PhysicsBodyType_RigidDynamic,
                     PhysicsBoundType_Box,
                     m_currentPosition,
                     1,
                     0.2);
    m_pMesh->GetResource<XCMesh>()->GetBounds(desc.m_boundVolumeDesc.m_boundDesc.m_boundBoxDesc.m_min, desc.m_boundVolumeDesc.m_boundDesc.m_boundBoxDesc.m_max);
    m_physicsFeature = playground.CreatePhysicsFeature(desc);

    //Load done. Shoot the bullet
    Shoot(5000.0f);
}

void Bullet::Shoot(f32 scalarForce)
{
    m_physicsFeature->GetTyped<RigidBody>()->AddForce(m_look * scalarForce);
}

IActor::ActorReturnState Bullet::Update(f32 dt)
{
    m_currentPosition = m_physicsFeature->GetTransformedPosition();

    m_MTranslation = MatrixTranslate(m_currentPosition.Get<X>(), m_currentPosition.Get<Y>(), m_currentPosition.Get<Z>());

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    PhysicsActor::Update(dt);

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    if (m_currentPosition.Get<Y>() < -10.0f)
    {
        Invalidate();
    }

    return PhysicsActor::Update(dt);
}

bool Bullet::Draw(RenderContext& renderContext)
{
#if defined(XCGRAPHICS_DX11)
    renderContext.GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#endif
    u32 stride = sizeof(VertexPosNormTex);
    u32 offset = 0;
    
    // Set constants
    ICamera& cam = renderContext.GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
        MatrixTranspose(m_World).GetUnaligned(),
        MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
        MatrixInverseTranspose(m_World).GetUnaligned(),
        XCMatrix4().GetUnaligned(),
        m_material
    };

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);

    return PhysicsActor::Draw(renderContext);
}

IActor::ActorReturnState Bullet::Destroy()
{
    return PhysicsActor::Destroy();
}