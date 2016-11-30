/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gun.h"

#include "Engine/GameplayBase/SceneGraph.h"
#include "Engine/GameplayBase/Actors/GameActorsFactory.h"
#include "Engine/Resource/ResourceManager.h"

#include "Gameplay/GameActors/Weapons/Bullets/Bullet.h"
#include "Gameplay/XCCamera/XCCameraManager.h"

#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCMesh/XCMesh.h"

Gun::Gun(void)
{
}

Gun::~Gun(void)
{
}

IActor::ActorReturnState Gun::LoadMetaData( const void* metaData )
{
    SimpleActor::LoadMetaData(metaData);

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource("Gun");

    m_directInput = (XCInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient = XCVec4Unaligned(0.1f, 0.1f, 0.1f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    //m_currentPosition = initialPosition;

    m_useShaderType = ShaderType_LightTexture;

    m_secondaryLookAxis  = XCVec4::XCFloat4ZeroVector;
    m_secondaryUpAxis    = XCVec4::XCFloat4ZeroVector;
    m_secondaryRightAxis = XCVec4::XCFloat4ZeroVector;

    //Bullets
    m_noOfBullets = 100;
    m_recoilDelta = 0.0f;
    m_recoilMaxTime = 0.5f;
    m_canShootBullet = true;

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState Gun::Load()
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

    return SimpleActor::Load();
}

IActor::ActorReturnState Gun::OnLoaded()
{
    IActor::ActorReturnState result = SimpleMeshActor::OnLoaded();

    //Since resources are interleaved. We need to wait for them to be loaded. Further way to improve this is, having callbacks when resource is loaded.
    if (m_pMesh == nullptr || (m_pMesh && m_pMesh->GetResource<XCMesh>()->IsLoaded()))
    {
        result = IActor::ActorReturnState_Success;
    }
    else
    {
        result = IActor::ActorReturnState_Processing;
    }

    return result;
}

IActor::ActorReturnState Gun::Update(f32 dt)
{
    UpdateGunRecoil(dt);

    UpdateOffsets(dt);

    m_MTranslation = MatrixTranslate(m_currentPosition);

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    return SimpleMeshActor::Update(dt);
}

void Gun::UpdateGunRecoil(f32 dt)
{
    if (!m_canShootBullet)
    {
        m_recoilDelta += dt;
        
        if (m_recoilDelta > m_recoilMaxTime)
        {
            m_recoilDelta = 0.0f;
            m_canShootBullet = true;
        }
    }
}

void Gun::UpdateOffsets(f32 dt)
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
}

void Gun::ApplyOffsetRotation()
{
    m_transformedRotation *= GetOffsetRotation();
}

bool Gun::Draw(RenderContext& renderContext)
{
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
    return SimpleMeshActor::Draw(renderContext);
}

void Gun::CheckInput()
{
    if (m_canShootBullet && m_directInput->MouseButtonDown(MOUSE_INPUT_BUTTON_L))
    {
        m_canShootBullet = false;

        //Left click
        //Spawn new bullet and shoot
        ShootBullet("Bullet", m_currentPosition, m_secondaryLookAxis);
    }
}

void Gun::ShootBullet(std::string bulletActorType, XCVec3& startPosition, XCVec3& target)
{
    GameActorsFactory& actorFactory = SystemLocator::GetInstance()->RequestSystem<GameActorsFactory>("GameActorsFactory");
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    SceneGraph& worldSystem = (SceneGraph&)SystemLocator::GetInstance()->RequestSystem("SceneGraph");

    Bullet* bullet = (Bullet*)actorFactory.CreateActor(bulletActorType);

    if (bullet)
    {
        bullet->LoadMetaData(nullptr);
        worldSystem.RequestLoadActor(bullet);
    }
}

IActor::ActorReturnState Gun::Destroy()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);

    return SimpleMeshActor::Destroy();
}