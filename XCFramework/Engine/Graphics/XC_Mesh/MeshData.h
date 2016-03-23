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

    void    init();
    void    destroy();

    std::string getSubMeshName() { return m_objectName; }
    void        createConstantBuffers();
    void    addVertex(float x, float y, float z);
    void    addVertex(Vertex& vert);
    void    addMapCoord(float u, float v);
    void    addFace(Face& face);
    void    addFace(unsigned short a, unsigned short b, unsigned short c, unsigned short flag);
    void    addBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight);

    int     getNoOfVertices() { return m_noOfVertices; }
    int     getNoOfFaces() { return m_noOfFaces; }

    void    setNoOfVertices(unsigned int noVerts) { m_noOfVertices = noVerts; }
    void    setNoOfFaces(unsigned int noFaces) { m_noOfFaces = noFaces; }
    void    setObjectName(const char* objName) { m_objectName = objName; }
    void    setNoOfBones(unsigned int noBones) { m_noOfBones = noBones; }

    void    setGeometryTranslation(XCVec3Unaligned trans) { m_initialTranslation = trans; }
    void    setGeometryRotation(XCVec3Unaligned rot) { m_initialRotation = rot; }
    void    setGeometryScaling(XCVec3Unaligned scale) { m_initialScaling = scale; }

    const XCVec3Unaligned& getGeometryTranslation()     { return m_initialTranslation; }
    const XCVec3Unaligned& getGeometryRotation()        { return m_initialRotation; }
    const XCVec3Unaligned& getGeometryScaling()         { return m_initialScaling; }

    //Get Geometry buffers
    void*                       getVertexBuffer()   { return m_vertexBuffer; }
    void*                       getInstanceBuffer() { return m_instanceBuffer; }
    IndexBuffer<unsigned int>&  getIndexBuffer()    { return m_indexBuffer; }

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

    D3DConstantBuffer*                  m_boneBuffer;
    D3DConstantBuffer*                  m_constantBuffer;
private:

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

    //Instance count
    unsigned int                       m_instanceCount;
};