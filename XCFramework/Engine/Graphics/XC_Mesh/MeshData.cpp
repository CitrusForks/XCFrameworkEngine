/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "MeshData.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Engine/Graphics/XC_Graphics.h"

MeshData::MeshData(ShaderType shaderType)
{
    m_shaderType = shaderType;
    init();
}

void MeshData::destroy()
{
    m_vertices.clear();
    m_mapCoord.clear();
    m_faces.clear();
}

void MeshData::init()
{
    m_objectName = new char[20];
    m_noOfVertices = 0;
    m_noOfFaces = 0;

    m_vertices.clear();
    m_faces.clear();
    m_mapCoord.clear();

    m_initialTranslation = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialRotation = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialScaling = XCVec3Unaligned(1.0f, 1.0f, 1.0f);

    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    XCShaderHandle* shaderHandle = (XCShaderHandle*)graphicsSystem.GetShaderManagerSystem().GetShader(m_shaderType);
    m_vertexBuffer = shaderHandle->createVertexBuffer();
}

void MeshData::createConstantBuffers()
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_constantBuffer = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectInstanced)));
    memset(m_constantBuffer->m_cbDataBegin, 0, sizeof(cbPerObjectInstanced));
}

void MeshData::addVertex(float x, float y, float z)
{
    Vertex v = { x, y, z };
    m_vertices.push_back(v);
}

void MeshData::addVertex(Vertex& vert)
{
    m_vertices.push_back(vert);
}

void MeshData::addFace(Face& face)
{
    m_faces.push_back(face);
}

void MeshData::addMapCoord(float u, float v)
{
    MapCoord mc = { u, v };
    m_mapCoord.push_back(mc);
}

void MeshData::addFace(unsigned short a, unsigned short b, unsigned short c, unsigned short flag)
{
    Face f = { a, b, c, flag };
    m_faces.push_back(f);
}

void MeshData::addBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight)
{
    //m_boneInfos.push_back(BoneInfo(boneIndex, boneWeight));
}