/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "MeshData.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Graphics/XC_Graphics.h"

MeshData::MeshData(ShaderType shaderType)
{
    m_shaderType = shaderType;
    Init();
}

void MeshData::Destroy()
{
    m_vertices.clear();
    m_mapCoord.clear();
    m_faces.clear();
}

void MeshData::Init()
{
    m_objectName = XCNEW(char)[20];
    m_noOfVertices = 0;
    m_noOfFaces = 0;

    m_vertices.clear();
    m_faces.clear();
    m_mapCoord.clear();

    m_width = 0.0f;

    m_initialTranslation = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialRotation = XCVec3Unaligned(0.0f, 0.0f, 0.0f);
    m_initialScaling = XCVec3Unaligned(1.0f, 1.0f, 1.0f);

    XC_Graphics& graphicsSystem = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
    XCShaderHandle* shaderHandle = (XCShaderHandle*)graphicsSystem.GetShaderManagerSystem().GetShader(m_shaderType);
    m_vertexBuffer = shaderHandle->CreateVertexBuffer();
}

void MeshData::AddVertex(f32 x, f32 y, f32 z)
{
    Vertex v = { x, y, z };
    m_vertices.push_back(v);
}

void MeshData::AddVertex(Vertex& vert)
{
    m_vertices.push_back(vert);
}

void MeshData::AddFace(Face& face)
{
    m_faces.push_back(face);
}

void MeshData::AddMapCoord(f32 u, f32 v)
{
    MapCoord mc = { u, v };
    m_mapCoord.push_back(mc);
}

void MeshData::AddFace(u16 a, u16 b, u16 c, u16 flag)
{
    Face f = { a, b, c, flag };
    m_faces.push_back(f);
}

void MeshData::AddBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight)
{
    //m_boneInfos.push_back(BoneInfo(boneIndex, boneWeight));
}