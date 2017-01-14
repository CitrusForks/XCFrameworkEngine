/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/DataStructures/Trees/XCTree.h"
#include "Graphics/XCShaders/XCVertexFormat.h"

class IPhysicsBoundVolume;
class OBBBoundVolume;

//HeightfieldQuad is a Node, which can contain child nodes
class HeightfieldQuad : public NTreeNode<HeightfieldQuad*, BreathFirstSearch>
{
public:

    HeightfieldQuad()
    {
        m_vMin = XCVec4(Infinity, Infinity, Infinity, 1);
        m_vMax = XCVec4(-Infinity, -Infinity, -Infinity, 1);
    }

    HeightfieldQuad(i32 rowStart, i32 rowEnd, i32 colStart, i32 colEnd, i32 width, XCVec4& min, XCVec4& max)
    {
        m_rowStart = rowStart;
        m_rowEnd = rowEnd;
        m_colStart = colStart;
        m_colEnd = colEnd;
        m_totalWidth = width;
        m_vMin = min;
        m_vMax = max;

        Logger("[HeightfieldQuad] RowStart - %d RowEnd - %d ColStart - %d ColEnd - %d\n", m_rowStart, m_rowEnd, m_colStart, m_colEnd);
    }

    ~HeightfieldQuad()
    {
    }

    bool                   ComputeQuad(i32 row, i32 col, XCVec4& pos);
    void                   ComputeOBBForAllQuads();
    const HeightfieldQuad* GetQuadCollidingWithOBB(const OBBBoundVolume* bbox) const;

    void                   Transform(const XCVec4& translate, const XCVec4& orientation);

    void                   Update(f32 dt);

    XCVec4                 m_vMin;
    XCVec4                 m_vMax;

    i32                    m_totalWidth;
    i32                    m_rowStart;
    i32                    m_rowEnd;
    i32                    m_colStart;
    i32                    m_colEnd;

    IPhysicsBoundVolume*   m_bbox;
};

class HeightfieldOBBHierarchy
{
public:
    static const i32            NO_OF_QUADS_EACH_LEVEL = 4;

    HeightfieldOBBHierarchy(const PhysicsDesc::PhysicsHeightFieldDesc& heightfieldDesc);
    ~HeightfieldOBBHierarchy();

    void                        CreateHeightfieldOBBHierarchy();
    void                        CreateHeightfieldOBBHierarchy(i32 levels);

    void                        Transform(const XCVec4& translate, const XCVec4& orientation);

    void                        Update(f32 dt);

    XCVec4                      GetPointOfContactWithOBB(const IPhysicsBoundVolume* bboxActor) const;

protected:
    void                        ComputeQuads();
    void                        ComputeOBBForAllQuads();

    const HeightfieldQuad*      GetQuadCollidingWithOBB(const OBBBoundVolume* bbox) const;

private:
    XCTree<HeightfieldQuad*>*   m_heightfieldQuadTree;

    u32                         m_rows;
    u32                         m_cols;

    void*                       m_vertexBuffer;
    VertexFormat                m_vertexFormat;
};