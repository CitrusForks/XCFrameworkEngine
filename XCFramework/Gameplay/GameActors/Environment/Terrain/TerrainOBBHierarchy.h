/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Base/DataStructures/Trees/XCTree.h"

class RenderContext;
class OrientedBoundingBox;
class RenderableOBB;

//TerrainQuad is a Node, which can contain child nodes
struct TerrainQuad
{
    TerrainQuad()
    {
        m_vMin = XCVec4(Infinity, Infinity, Infinity, 1);
        m_vMax = XCVec4(-Infinity, -Infinity, -Infinity, 1);
    }

    TerrainQuad(i32 rowStart, i32 rowEnd, i32 colStart, i32 colEnd, i32 width, XCVec4& min, XCVec4& max)
    {
        m_rowStart = rowStart;
        m_rowEnd = rowEnd;
        m_colStart = colStart;
        m_colEnd = colEnd;
        m_totalWidth = width;
        m_vMin = min;
        m_vMax = max;

        Logger("[TerrainQuad] RowStart - %d RowEnd - %d ColStart - %d ColEnd - %d\n", m_rowStart, m_rowEnd, m_colStart, m_colEnd);
    }

    ~TerrainQuad()
    {
    }

    bool           ComputeQuad(i32 row, i32 col, XCVec4& pos);
    void           ComputeOBBForAllQuads();
    TerrainQuad*   GetQuadCollidingWithOBB(OrientedBoundingBox* bbox);
    void           Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

    void           Update(f32 dt);
    void           Draw(RenderContext& context);

    XCVec4         m_vMin;
    XCVec4         m_vMax;

    i32            m_totalWidth;
    i32            m_rowStart;
    i32            m_rowEnd;
    i32            m_colStart;
    i32            m_colEnd;

    std::unique_ptr<RenderableOBB>   m_bbox;
};

class TerrainOBBHierarchy
{
public:
    static const i32     NO_OF_QUADS_EACH_LEVEL = 4;

    TerrainOBBHierarchy();
    ~TerrainOBBHierarchy();

    void                    CreateTerrainOBBHierarchy(i32 rowStart, i32 totalRows, i32 colStart, i32 totalColumns, i32 totalWidth);
    void                    CreateTerrainOBBHierarchy(i32 levels, i32 rowStart, i32 totalRows, i32 colStart, i32 totalColumns, i32 totalWidth);
    void                    ComputeQuad(i32 row, i32 col, XCVec4& pos);
    void                    ComputeOBBForAllQuads();
    void                    Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

    void                    Update(f32 dt);
    void                    Draw(RenderContext& context);

    TerrainQuad*            GetQuadCollidingWithOBB(OrientedBoundingBox* bbox);

protected:
    //Recursive calls to traverse the tree
    void                    ComputeQuad(XCTreeNode<TerrainQuad*>& node, i32 row, i32 col, XCVec4& pos);
    void                    ComputeOBBForAllQuads(XCTreeNode<TerrainQuad*>& node);
    TerrainQuad*            GetQuadCollidingWithOBB(XCTreeNode<TerrainQuad*>& node, OrientedBoundingBox* bbox);

    void                    Update(XCTreeNode<TerrainQuad*>& node, f32 dt);
    void                    Draw(XCTreeNode<TerrainQuad*>& node, RenderContext& context);

    void                    Transform(XCTreeNode<TerrainQuad*>& node, XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

private:
    XCTree<TerrainQuad*>*   m_terrainQuadTree;
};