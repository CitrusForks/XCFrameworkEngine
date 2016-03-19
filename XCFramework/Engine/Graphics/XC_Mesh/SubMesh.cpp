/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "SubMesh.h"

SubMesh::SubMesh(EVertexFormat format)
{
    m_vertexFormatType = format;
    init();
}

void SubMesh::destroy()
{
    delete(m_objectName);

    m_vertices.clear();
    m_mapCoord.clear();
    m_faces.clear();
}

void SubMesh::init()
{
    m_initialTranslation = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialRotation    = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialScaling     = XCVec3Unaligned(0.0f, 0.0f, 0.0f);

    m_objectName = new char[20];
    m_noOfVertices = 0;
    m_noOfFaces = 0;

    m_vertices.clear();
    m_faces.clear();
    m_mapCoord.clear();

    createVertexBuffer();
}

void SubMesh::createConstantBuffers()
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_constantBuffer = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectInstanced)));
    memset(m_constantBuffer->m_cbDataBegin, 0, sizeof(cbPerObjectInstanced));
}

void SubMesh::createVertexBuffer()
{
    switch (m_vertexFormatType)
    {
    case VertexFormat_PositionNormalTexture:
        m_vertexBuffer = new VertexBuffer<VertexPosNormTex>();
        break;

    case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
        m_vertexBuffer = new VertexBuffer<VertexPosNormTexBIndexBWeight>();
        break;

    case VertexFormat_PositionColorInstanceIndex:
        m_vertexBuffer = new VertexBuffer<VertexPosColorInstanceIndex>();
        break;

    default:
        XCASSERT(false);
    }
}

void SubMesh::addVertex(float x, float y, float z)
{
    Vertex v = { x, y, z };
    m_vertices.push_back(v);
}

void SubMesh::addVertex(Vertex& vert)
{
    m_vertices.push_back(vert);
}

void SubMesh::addFace(Face& face)
{
    m_faces.push_back(face);
}

void SubMesh::addMapCoord(float u, float v)
{
    MapCoord mc = { u, v };
    m_mapCoord.push_back(mc);
}

void SubMesh::addFace(unsigned short a, unsigned short b, unsigned short c, unsigned short flag)
{
    Face f = { a, b, c, flag };
    m_faces.push_back(f);
}

void SubMesh::addBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight)
{
    //m_boneInfos.push_back(BoneInfo(boneIndex, boneWeight));
}