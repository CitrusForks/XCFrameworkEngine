/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "SubMesh.h"

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