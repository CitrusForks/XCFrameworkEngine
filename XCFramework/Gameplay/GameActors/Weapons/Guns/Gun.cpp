/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Gun.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"

#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Gameplay/World.h"

#include "Gameplay/GameActors/Weapons/Bullets/Bullet.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"


Gun::Gun(void)
{
}

Gun::~Gun(void)
{
}

void Gun::PreLoad(IActor* parentActor, XCVec3 initialPosition, XCMesh* pMesh)
{
    SubActor::Init(parentActor);

    m_pMesh = pMesh;

    m_directInput = (DirectInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    //Get initial position
    m_currentPosition = XMLoadFloat3(&initialPosition);

    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;

    m_secondaryLookAxis = XMVectorZero();
    m_secondaryUpAxis = XMVectorZero();
    m_secondaryRightAxis = XMVectorZero();

    //Bullets
    m_noOfBullets = 100;
    m_recoilDelta = 0.0f;
    m_recoilMaxTime = 0.5f;
    m_canShootBullet = true;
}

void Gun::Load()
{
    m_currentPosition += GetOffsetPosition();

    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MInitialRotation = XMMatrixRotationY(XM_PIDIV2);
    m_MInitialRotation *= XMMatrixRotationZ(XM_PIDIV2);

    m_transformedRotation = m_MRotation;
    m_MRotation = m_transformedRotation * m_MInitialRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_secondaryLookAxis = m_look;
    m_secondaryUpAxis = m_up;
    m_secondaryRightAxis = m_right;
}

void Gun::Update(float dt)
{
    UpdateGunRecoil(dt);

    UpdateOffsets(dt);

    m_MTranslation = XMMatrixTranslation(XMVectorGetX(m_currentPosition), XMVectorGetY(m_currentPosition), XMVectorGetZ(m_currentPosition));

    m_MRotation = m_MInitialRotation * m_transformedRotation;

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->Update(dt);

    SimpleMeshActor::Update(dt);
}

void Gun::UpdateGunRecoil(float dt)
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

void Gun::UpdateOffsets(float dt)
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
}

void Gun::ApplyOffsetRotation()
{
    m_transformedRotation *= GetOffsetRotation();
}

void Gun::Draw(RenderContext& context)
{
    // Set constants
    XC_CameraManager* cam = (XC_CameraManager*)&SystemLocator::GetInstance()->RequestSystem("CameraManager");
    PerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam->GetViewMatrix() * cam->GetProjMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    m_pMesh->DrawInstanced(perObject);
    SimpleMeshActor::Draw(context);
}

void Gun::CheckInput()
{
    if (m_canShootBullet && m_directInput->MouseButtonDown(MOUSE_INPUT_BUTTON_L))
    {
        m_canShootBullet = false;

        //Left click
        //Spawn new bullet and shoot
        XCVec3 position, target;
        XMStoreFloat3(&position, m_currentPosition);
        XMStoreFloat3(&target, m_secondaryLookAxis);

        ShootBullet("Bullet", position, target);
    }
}

void Gun::ShootBullet(std::string bulletActorType, XCVec3 startPosition, XCVec3 target)
{
    GameActorsFactory& actorFactory = (GameActorsFactory&)SystemLocator::GetInstance()->RequestSystem("GameActorsFactory");
    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    World& worldSystem = (World&)SystemLocator::GetInstance()->RequestSystem("World");

    Bullet* bullet = (Bullet*)actorFactory.CreateActor(bulletActorType);

    if (bullet)
    {
        bullet->PreLoad(startPosition, target, (XCMesh*)resMgr.GetResource("PistolBullet"));
        worldSystem.RequestAddActor(bullet);
    }
}

void Gun::Destroy()
{
    SimpleMeshActor::Destroy();
}