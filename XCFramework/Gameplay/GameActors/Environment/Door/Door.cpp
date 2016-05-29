/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Door.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Camera/XC_CameraManager.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"

Door::Door(void)
{
    m_actorType = GAMEACTOR_DOOR;
    m_directInput = (DirectInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = ShaderType_LightTexture;
    m_useRenderWorkerType = WorkerType_XCMesh;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;
}

Door::~Door(void)
{
}

void Door::PreLoad(const void* fbBuffer)
{
    const FBDoor* doorBuff = (FBDoor*)fbBuffer;
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(doorBuff->XCMeshResourceName()->c_str());

    PhysicsActor::PreLoad(fbBuffer);
}

void Door::PreLoad(XCVec3 _initialPosition, std::string pMesh)
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(pMesh.c_str());
    m_initialPosition = _initialPosition;
}

void Door::Load()
{
    PhysicsActor::Load();
}

void Door::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(XMLoadFloat3(&m_initialPosition), XMLoadFloat3(&vec), XMLoadFloat3(&vec), 1000, (float)0.2);
}

void Door::Update(float dt)
{
    if (m_directInput->KeyDown(INPUT_KEY_I))
    {
        //m_initialPosition.z++;
    }

    if (m_directInput->KeyDown(INPUT_KEY_K))
    {
        //m_initialPosition.z--;
    }

    Integrator(dt);
    ClearForce();

    m_MTranslation = XMMatrixTranslation(m_initialPosition.x, m_initialPosition.y, m_initialPosition.z);

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    PhysicsActor::Update(dt);
}

void Door::Draw(RenderContext& context)
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

    m_pMesh->GetResource<XCMesh>()->DrawInstanced(perObject);
    PhysicsActor::Draw(context);
}

void Door::Destroy()
{
    PhysicsActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);
}