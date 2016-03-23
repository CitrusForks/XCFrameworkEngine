/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "OrientedBoundingBox.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Graphics/RenderContext.h"

OrientedBoundingBox::OrientedBoundingBox()
{
}

OrientedBoundingBox::~OrientedBoundingBox()
{
}

void OrientedBoundingBox::Init()
{
    CubeMesh::Init();
}

void OrientedBoundingBox::CreateBoundBox()
{
    XCVecIntrinsic4 orientation = XMQuaternionRotationMatrix(XMMatrixIdentity());
    XCVec4 orient;
    XMStoreFloat4(&orient, orientation);
    m_bBox = BoundingOrientedBox(XMVectorToXMFloat3(&m_boxCenter), XMVectorToXMFloat3(&m_boxExtends), orient);
}

void OrientedBoundingBox::CreateBoundBox(XCVecIntrinsic4 min, XCVecIntrinsic4 max)
{
    m_boxCenter = 0.5f * (min + max);
    m_boxExtends = 0.5f * (max - min);
    CreateBoundBox();
}

void OrientedBoundingBox::CreateBoundBox(OrientedBoundingBox* const boundBox)
{
    m_boxCenter = XMLoadFloat3(&boundBox->m_bBox.Center);
    m_boxExtends = XMLoadFloat3(&boundBox->m_bBox.Extents);
    CreateBoundBox();
}

void OrientedBoundingBox::Load()
{
    m_rasterType = RasterType_FillWireframe;
    CubeMesh::Load();
}

void OrientedBoundingBox::Transform(XCMatrix4 translateMat, XCMatrix4 rotateMatrix)
{
    BoundingOrientedBox box;
    XCVecIntrinsic4 rotate = XMQuaternionRotationMatrix(rotateMatrix);

#if defined(XC_ORBIS)
    m_bBox.Transform(m_TransformedBox, 1.0f, rotate, translateMat.getRow(3));
#else
    m_bBox.Transform(m_TransformedBox, 1.0f, rotate, translateMat.r[3]);
#endif
}

void OrientedBoundingBox::Update(float dt)
{

    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");
    
#if defined(XC_ORBIS)
    m_MScaling = XMMatrixScaling(m_TransformedBox.Extents.getX(), m_TransformedBox.Extents.getY(), m_TransformedBox.Extents.getZ());
    m_MRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_TransformedBox.Orientation));
    m_MTranslation = XMMatrixTranslation(m_TransformedBox.Center.getX(), m_TransformedBox.Center.getY(), m_TransformedBox.Center.getZ());
#else
    m_MScaling     = XMMatrixScaling(m_TransformedBox.Extents.x, m_TransformedBox.Extents.y, m_TransformedBox.Extents.z);
    m_MRotation    = XMMatrixRotationQuaternion(XMLoadFloat4(&m_TransformedBox.Orientation));
    m_MTranslation = XMMatrixTranslation(m_TransformedBox.Center.x, m_TransformedBox.Center.y, m_TransformedBox.Center.z);
#endif

    CubeMesh::Update(dt);
    
    /* Another way of updating the world.
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

    for(unsigned int vertexIndex = 0; vertexIndex < m_vertices.size(); vertexIndex++)
    {
        XMVECTOR vector = XMLoadFloat3(&m_vertices[vertexIndex].Pos);
        XMFLOAT3 value;
        XMStoreFloat3(&value, XMVector3Transform(vector, matWorld));
        
        VertexPosNormTex out = m_vertices[vertexIndex];
        out.Pos = value;
        v[vertexIndex] = out;
    }
    graphicsSystem.GetDeviceContext()->Unmap(m_pVB, 0);*/
}

void OrientedBoundingBox::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillWireframe);
    CubeMesh::Draw(context);
}