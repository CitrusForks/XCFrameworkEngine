/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "RenderableOBB.h"
#include "Graphics/RenderContext.h"
#include "Graphics/XCMesh/XCMesh.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"

#include "Engine/Resource/ResourceManager.h"

RenderableOBB::RenderableOBB()
{
}

RenderableOBB::~RenderableOBB()
{
    Destroy();
}

void RenderableOBB::Init()
{
#if defined(DEBUG_OBB)
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_cubeMesh = &resMgr.AcquireResource("CubeMesh");

    m_cubeMesh->GetResource<XCMesh>()->SetRasterType(RasterType_FillWireframe);
#endif

    m_material.Ambient = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);
}

void RenderableOBB::Update(f32 dt)
{
#if defined(DEBUG_OBB)
    m_MScaling     = MatrixScale(m_TransformedBox.Extents.x, m_TransformedBox.Extents.y, m_TransformedBox.Extents.z);
    m_MRotation    = MatrixRotationQuaternion(XCVec4(m_TransformedBox.Orientation));
    m_MTranslation = MatrixTranslate(m_TransformedBox.Center.x, m_TransformedBox.Center.y, m_TransformedBox.Center.z);

    m_World = m_MScaling * m_MRotation * m_MTranslation;
#endif

#if defined(UNUSED)
    //Another way of updating the world.
    XMVECTOR position = XMLoadFloat3(&m_TransformedBox.Center);
    XMMATRIX matWorld = XMMatrixScaling(m_TransformedBox.Extents.x, m_TransformedBox.Extents.y, m_TransformedBox.Extents.z);

    //Calculate orientation
    XMMATRIX rotationMat = XMMatrixRotationQuaternion(XMLoadFloat4(&m_TransformedBox.Orientation));

    //This order is necessary, above matWorld has scaling values. Follow SRT!!
    matWorld = matWorld * rotationMat;
    matWorld.r[3] = XMVectorSelect(matWorld.r[3], position, XMVectorSelectControl(1, 1, 1, 0));

    m_World = matWorld;

    //Not required. The m_World contains this modified SRT.
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(graphicsSystem.GetDeviceContext()->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    VertexPosNormTex* v = (VertexPosNormTex*)mappedData.pData;

    for(u32 vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++)
    {
        XMVECTOR vector = XMLoadFloat3(&m_vertices[vertexIndex].Pos);
        XMFLOAT3 value;
        XMStoreFloat3(&value, XMVector3Transform(vector, matWorld));
        
        VertexPosNormTex out = m_vertices[vertexIndex];
        out.Pos = value;
        v[vertexIndex] = out;
    }
    graphicsSystem.GetDeviceContext()->Unmap(m_pVB, 0);
#endif
}

void RenderableOBB::Draw(RenderContext& context)
{
#if defined(DEBUG_OBB)
    // Set constants
    ICamera& cam = context.GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = 
    {
        MatrixTranspose(m_World).GetUnaligned(),
        MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
        MatrixInverseTranspose(m_World).GetUnaligned(),
        XCMatrix4::XCMatrixIdentity.GetUnaligned(),
        m_material
    };

    m_cubeMesh->GetResource<XCMesh>()->DrawInstanced(perObject);
#endif
}

void RenderableOBB::Destroy()
{
#if defined(DEBUG_OBB)
    if (m_cubeMesh)
    {
        ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
        resMgr.ReleaseResource(m_cubeMesh);

        m_cubeMesh = nullptr;
    }
#endif
}
