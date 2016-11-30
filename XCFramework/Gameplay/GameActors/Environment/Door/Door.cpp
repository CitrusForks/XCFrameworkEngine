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

    m_useShaderType = ShaderType_LightTexture;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_ORIENTEDBOUNDINGBOX;
}

Door::~Door(void)
{
}

IActor::ActorReturnState Door::LoadMetaData( const void* metaData )
{
    const FBDoor* doorBuff = (FBDoor*)metaData;
    
    PhysicsActor::LoadMetaData(doorBuff->Base());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_pMesh = &resMgr.AcquireResource(doorBuff->XCMeshResourceName()->c_str());

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState Door::Load()
{
    return PhysicsActor::Load();
}

void Door::SetInitialPhysicsProperties()
{
    PhysicsActor::SetInitialPhysicsProperties();
    InitXPhysics(XCVec4(m_initialPosition), XCVec4(), XCVec4(), 1000, (f32)0.2);
}

IActor::ActorReturnState Door::Update(f32 dt)
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

    return PhysicsActor::Update(dt);
}

bool Door::Draw(RenderContext& renderContext)
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
    PhysicsActor::Draw(renderContext);

    return true;
}

IActor::ActorReturnState Door::Destroy()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    resMgr.ReleaseResource(m_pMesh);

    return PhysicsActor::Destroy();
}