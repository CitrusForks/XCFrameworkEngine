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

class SubMesh
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

    SubMesh(EVertexFormat format)
    {
        m_vertexFormatType = format;
        init();
    }

    template<class T>
    VertexBuffer<T>&         getVertexBuffer()
    {
        return *(VertexBuffer<T>*)m_vertexBuffer;
    }

    template<class T>
    VertexBuffer<T>&         getInstanceBuffer()
    {
        return *(VertexBuffer<T>*)m_instanceBuffer;
    }

    std::string getSubMeshName() { return m_objectName; }

    IndexBuffer<unsigned int>& getIndexBuffer() { return m_indexBuffer; }

    int     getNoOfVertices() { return m_noOfVertices; }
    int     getNoOfFaces() { return m_noOfFaces; }

    void    setNoOfVertices(unsigned int noVerts) { m_noOfVertices = noVerts; }
    void    setNoOfFaces(unsigned int noFaces) { m_noOfFaces = noFaces; }
    void    setObjectName(const char* objName) { strcpy(m_objectName, objName); }
    void    setNoOfBones(unsigned int noBones) { m_noOfBones = noBones; }

    void    addVertex(float x, float y, float z);
    void    addVertex(Vertex& vert);
    void    addMapCoord(float u, float v);
    void    addFace(Face& face);
    void    addFace(unsigned short a, unsigned short b, unsigned short c, unsigned short flag);
    void    addBoneInfo(XCVec4Unaligned boneIndex, XCVec4Unaligned boneWeight);

    void    init()
    {
        m_objectName = new char[20];
        m_noOfVertices = 0;
        m_noOfFaces = 0;

        m_vertices.clear();
        m_faces.clear();
        m_mapCoord.clear();

        createVertexBuffer();
    }

    void createConstantBuffers()
    {
        SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
        m_constantBuffer = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(cbPerObjectInstanced)));
    }

    void    destroy()
    {
        delete(m_objectName);

        m_vertices.clear();
        m_mapCoord.clear();
        m_faces.clear();
    }

    std::vector<Vertex>                m_vertices;
    std::vector<MapCoord>              m_mapCoord;
    std::vector<Face>                  m_faces;

#if CUSTOM_ANIMATION
    BoneHierarchy                      m_boneHierarchy;
    std::vector<BoneNode>              m_boneNodes;
    std::vector<BoneInfo>              m_boneInfos;
    MeshNode*                          m_meshNodeStructure;
#endif

    D3DConstantBuffer*                 m_boneBuffer;
    D3DConstantBuffer*                 m_constantBuffer;

protected:

    void                                createVertexBuffer()
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
            m_vertexBuffer   = new VertexBuffer<VertexPosColorInstanceIndex>();
            break;

        default:
            XCASSERT(false);
        }
    }

private:
    void*                              m_vertexBuffer;
    void*                              m_instanceBuffer;
    IndexBuffer<unsigned int>          m_indexBuffer;

    unsigned int                       m_noOfBones;
    unsigned int                       m_noOfVertices;
    unsigned int                       m_noOfFaces;

    char*                              m_objectName;
    EVertexFormat                      m_vertexFormatType;
};