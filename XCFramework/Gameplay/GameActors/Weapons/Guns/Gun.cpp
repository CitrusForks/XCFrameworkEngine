/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Gun.h"

#include "Gameplay/World.h"
#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Gameplay/GameActors/Weapons/Bullets/Bullet.h"
#include "Gameplay/XC_Camera/XC_CameraManager.h"

#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"

Gun::Gun(void)
{
}

Gun::~Gun(void)
{
}

void Gun::PreLoad(IActor* parentActor, XCVec3& initialPosition, std::string pMesh)
{
    SubActor::Init(parentActor);

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(pMesh.c_str());

    m_directInput = (DirectInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient  = XCVec4(0.1f, 0.1f, 0.1f, 1.0f);
    m_material.Diffuse  = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    m_currentPosition = initialPosition;

    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;

    m_secondaryLookAxis  = XCVec4(0, 0, 0, 0);
    m_secondaryUpAxis    = XCVec4(0, 0, 0, 0);
    m_secondaryRightAxis = XCVec4(0, 0, 0, 0);

    //Bullets
    m_noOfBullets = 100;
    m_recoilDelta = 0.0f;
    m_recoilMaxTime = 0.5f;
    m_canShootBullet = true;
}

void Gun::Load()
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

void Gun::Update(f32 dt)
{
    UpdateGunRecoil(dt);

    UpdateOffsets(dt);

    m_MTranslation = MatrixTranslate(m_currentPosition);

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    SimpleMeshActor::Update(dt);
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

void Gun::Draw(RenderContext& context)
{
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
    SimpleMeshActor::Draw(context);
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
    World& worldSystem = (World&)SystemLocator::GetInstance()->RequestSystem("World");

    Bullet* bullet = (Bullet*)actorFactory.CreateActor(bulletActorType);

    if (bullet)
    {
        bullet->PreLoad(startPosition, target, "PistolBullet");
        worldSystem.RequestAddActor(bullet);
    }
}

void Gun::Destroy()
{
    SimpleMeshActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);
}