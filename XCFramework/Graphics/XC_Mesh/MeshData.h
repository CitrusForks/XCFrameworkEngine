/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"

#include "BoneHierarchy.h"
#include "BoneInfo.h"

class MeshData
{
public:

    struct Vertex
    {
        f32 x;
        f32 y;
        f32 z;
    };

    struct MapCoord
    {
        f32 u;
        f32 v;
    };

    struct Face
    {
        Face(u16 x, u16 y, u16 z, u16 w = 0)
            : a(x)
            , b(y)
            , c(z)
            , flags(w)
        {}

        u16 a;
        u16 b;
        u16 c;

        u16 flags;
    };


    MeshData(ShaderType shaderType);

    void        Init();
    void        Destroy();

    void        AddVertex(f32 x, f32 y, f32 z);
    void        AddVertex(Vertex& vert);
    void        AddMapCoord(f32 u, f32 v);
    void        AddFace(Face& face);
    void        AddFace(u16 a, u16 b, u16 c, u16 flag);
    void        AddBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight);

    void        SetObjectName(const char* objName) { m_objectName = objName; }

    void        SetGeometryTranslation(XCVec3Unaligned trans) { m_initialTranslation = trans; }
    void        SetGeometryRotation(XCVec3Unaligned rot) { m_initialRotation = rot; }
    void        SetGeometryScaling(XCVec3Unaligned scale) { m_initialScaling = scale; }
    void        SetMeshAbsWidth(f32 width) { m_meshAbsWidth = width; }

    i32                    GetNoOfVertices() { return m_vertices.size(); }
    i32                    GetNoOfFaces()    { return m_faces.size(); }
    std::string            GetSubMeshName()  { return m_objectName; }
    f32                    GetMeshAbsWidth() { return m_meshAbsWidth; }

    const XCVec3Unaligned& GetGeometryTranslation()     { return m_initialTranslation; }
    const XCVec3Unaligned& GetGeometryRotation()        { return m_initialRotation; }
    const XCVec3Unaligned& GetGeometryScaling()         { return m_initialScaling; }

    //Get Geometry buffers
    void*                       GetVertexBuffer()   { return m_vertexBuffer; }
    void*                       GetInstanceBuffer() { return m_instanceBuffer; }
    IndexBuffer<u32>&           GetIndexBuffer()    { return m_indexBuffer; }

    //Raw Geometry containers. TODO : Clear them when buffers are created.
    std::vector<Vertex>                m_vertices;
    std::vector<MapCoord>              m_mapCoord;
    std::vector<Face>                  m_faces;

#if CUSTOM_ANIMATION
    BoneHierarchy                      m_boneHierarchy;
    std::vector<BoneNode>              m_boneNodes;
    std::vector<BoneInfo>              m_boneInfos;
    MeshNode*                          m_meshNodeStructure;
#endif


private:
    //These will be applied in model space of the mesh, that is transformation on vertex based to align with the world.
    XCVec3Unaligned                    m_initialScaling;
    XCVec3Unaligned                    m_initialRotation;
    XCVec3Unaligned                    m_initialTranslation;

    //Geometry Buffers
    IndexBuffer<u32>                   m_indexBuffer;

    void*                              m_vertexBuffer;
    void*                              m_instanceBuffer;

    std::string                        m_objectName;
    ShaderType                         m_shaderType;
    f32                                m_meshAbsWidth;
};