/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Engine/Graphics/IndexBuffer.h"
#include "Engine/Graphics/VertexBuffer.h"

#include "BoneHierarchy.h"
#include "BoneInfo.h"

class MeshData
{
public:

    struct Vertex
    {
        float x;
        float y;
        float z;
    };

    struct MapCoord
    {
        float           u;
        float           v;
    };

    struct Face
    {
        unsigned short a;
        unsigned short b;
        unsigned short c;

        unsigned short flags;
    };


    MeshData(ShaderType shaderType);

    void    Init();
    void    Destroy();

    std::string GetSubMeshName() { return m_objectName; }
    void    AddVertex(float x, float y, float z);
    void    AddVertex(Vertex& vert);
    void    AddMapCoord(float u, float v);
    void    AddFace(Face& face);
    void    AddFace(unsigned short a, unsigned short b, unsigned short c, unsigned short flag);
    void    AddBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight);

    int     GetNoOfVertices() { return m_noOfVertices; }
    int     GetNoOfFaces() { return m_noOfFaces; }

    void    SetNoOfVertices(unsigned int noVerts) { m_noOfVertices = noVerts; }
    void    SetNoOfFaces(unsigned int noFaces) { m_noOfFaces = noFaces; }
    void    SetObjectName(const char* objName) { m_objectName = objName; }
    void    SetNoOfBones(unsigned int noBones) { m_noOfBones = noBones; }

    void    SetGeometryTranslation(XCVec3Unaligned trans) { m_initialTranslation = trans; }
    void    SetGeometryRotation(XCVec3Unaligned rot) { m_initialRotation = rot; }
    void    SetGeometryScaling(XCVec3Unaligned scale) { m_initialScaling = scale; }

    const XCVec3Unaligned& GetGeometryTranslation()     { return m_initialTranslation; }
    const XCVec3Unaligned& GetGeometryRotation()        { return m_initialRotation; }
    const XCVec3Unaligned& GetGeometryScaling()         { return m_initialScaling; }

    //Get Geometry buffers
    void*                       GetVertexBuffer()   { return m_vertexBuffer; }
    void*                       GetInstanceBuffer() { return m_instanceBuffer; }
    IndexBuffer<unsigned int>&  GetIndexBuffer()    { return m_indexBuffer; }

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

    float                               m_width;

private:

    //Geometry Buffers
    void*                              m_vertexBuffer;
    void*                              m_instanceBuffer;
    IndexBuffer<unsigned int>          m_indexBuffer;

    unsigned int                       m_noOfBones;
    unsigned int                       m_noOfVertices;
    unsigned int                       m_noOfFaces;

    std::string                        m_objectName;
    ShaderType                         m_shaderType;

    //These will be applied in model space of the mesh, that is transformation on vertex based to align with the world.
    XCVec3Unaligned                    m_initialScaling;
    XCVec3Unaligned                    m_initialRotation;
    XCVec3Unaligned                    m_initialTranslation;
};