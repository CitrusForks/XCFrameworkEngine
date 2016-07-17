/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/BasicGeometry/RenderableOBB.h"

//Forward reference to the OBBHierarchy
class OBBHierarchy;

//TerrainQuad is a Node, which can contain child nodes
struct TerrainQuad
{
    TerrainQuad()
    {
        m_vMin = XCVec4(Infinity, Infinity, Infinity, 1);
        m_vMax = XCVec4(-Infinity, -Infinity, -Infinity, 1);
        m_nextChildNode = nullptr;
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
        m_nextChildNode = nullptr;

        Logger("[TerrainQuad] RowStart - %d RowEnd - %d ColStart - %d ColEnd - %d\n", m_rowStart, m_rowEnd, m_colStart, m_colEnd);
    }

    ~TerrainQuad()
    {
    }

    OBBHierarchy*  GetChildNode() { return m_nextChildNode.get(); }
    void           SetChildNodeOBB(std::unique_ptr<OBBHierarchy> obb) { m_nextChildNode = std::move(obb); }
    bool           HasChildNode();

    XCVec4         m_vMin;
    XCVec4         m_vMax;

    i32            m_totalWidth;
    i32            m_rowStart;
    i32            m_rowEnd;
    i32            m_colStart;
    i32            m_colEnd;

    std::unique_ptr<RenderableOBB>   m_bbox;
    std::unique_ptr<OBBHierarchy>    m_nextChildNode;
};

class OBBHierarchy
{
public:
    static const i32     NO_OF_QUADS_EACH_LEVEL = 4;

    OBBHierarchy();
    ~OBBHierarchy();

    void                 AddQuad(std::unique_ptr<TerrainQuad> bbox) { m_Quads.push_back(std::move(bbox)); }
    void                 CreateTerrainOBBHierarchy(i32 levels, i32 rowStart, i32 totalRows, i32 colStart, i32 totalColumns, i32 totalWidth);
    void                 ComputeQuad(i32 row, i32 col, XCVec4& pos);
    void                 ComputeOBBForAllQuads();
    void                 Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix);

    void                 Update(f32 dt);
    void                 Draw(RenderContext& context);

    TerrainQuad*         GetQuadCollidingWithOBB(OrientedBoundingBox* bbox);

private:
    std::vector<std::unique_ptr<TerrainQuad>>   m_Quads;
};