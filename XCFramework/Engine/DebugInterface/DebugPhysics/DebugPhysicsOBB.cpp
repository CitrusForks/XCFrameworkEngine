/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#if defined(DEBUG_PHYSICS_OBB)

#include "DebugPhysicsOBB.h"

#include "Engine/Resource/ResourceManager.h"
#include "Engine/Resource/ResourceHandle.h"

#include "Graphics/XCMesh/XCMesh.h"

#include "Physics/PhysicsPlayground.h"

DebugPhysicsOBB::DebugPhysicsOBB()
{
}

DebugPhysicsOBB::~DebugPhysicsOBB()
{
}

void DebugPhysicsOBB::Init()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_cubeMesh = &resMgr.AcquireResource("CubeMesh");

    m_cubeMesh->GetResource<XCMesh>()->SetRasterType(RasterType_FillWireframe);

    m_material.Ambient = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    m_renderableOBBs.clear();

    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().AddRenderableObject(this, GetBaseObjectId());

    m_isRenderable = true;
}

void DebugPhysicsOBB::Update(float dt)
{
    if(m_cubeMesh->GetResource<XCMesh>()->IsLoaded())
    {
        m_renderableOBBs.clear();

        PhysicsPlayground& plygrd = SystemLocator::GetInstance()->RequestSystem<PhysicsPlayground>("PhysicsPlayground");

        std::vector<PhysicsPlayground::OBBInfo> infoList;
        plygrd.GetOBBInfo(infoList);

        for(auto& obbInfo : infoList)
        {
            OBBWorldInfo world = {
                MatrixScale(obbInfo.m_extends) *
                MatrixRotationQuaternion(obbInfo.m_orientation) *
                MatrixTranslate(obbInfo.m_center)
            };
            m_renderableOBBs.push_back(world);
        }
    }
}

bool DebugPhysicsOBB::Draw(RenderContext& context)
{
    // Set constants
    ICamera& cam = context.GetGlobalShaderData().m_camera;

    for(auto& obbWorld : m_renderableOBBs)
    {
        PerObjectBuffer perObject =
        {
            MatrixTranspose(obbWorld.m_world).GetUnaligned(),
            MatrixTranspose(obbWorld.m_world * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
            MatrixInverseTranspose(obbWorld.m_world).GetUnaligned(),
            XCMatrix4::XCMatrixIdentity.GetUnaligned(),
            m_material
        };

        m_cubeMesh->GetResource<XCMesh>()->DrawInstanced(perObject);
    }

    return true;
}

void DebugPhysicsOBB::Destroy()
{
    XCGraphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XCGraphics>("GraphicsSystem");
    graphicsSystem.GetRenderingPool().RemoveRenderableObject(this, GetBaseObjectId());

    if(m_cubeMesh)
    {
        ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
        resMgr.ReleaseResource(m_cubeMesh);

        m_cubeMesh = nullptr;
    }
}

#endif