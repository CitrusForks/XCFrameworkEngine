/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Door.h"
#include "Engine/Resource/ResourceManager.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Gameplay/XCCamera/XCCameraManager.h"
#include "Graphics/XCMesh/XCMesh.h"

Door::Door(void)
{
    m_directInput = (XCInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");

    m_material.Ambient = XCVec4Unaligned(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = ShaderType_LightTexture;
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

void Door::PreLoad(XCVec3& _initialPosition, std::string pMesh)
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
    InitXPhysics(XCVec4(m_initialPosition), XCVec4(), XCVec4(), 1000, (f32)0.2);
}

void Door::Update(f32 dt)
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

    m_MTranslation = MatrixTranslate(XCVec4(m_initialPosition));

    m_World = m_MScaling * m_MRotation * m_MTranslation;

    m_pMesh->GetResource<XCMesh>()->Update(dt);

    PhysicsActor::Update(dt);
}

void Door::Draw(RenderContext& context)
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
    PhysicsActor::Draw(context);
}

void Door::Destroy()
{
    PhysicsActor::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);
}