/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/BasicGeometry/OrientedBoundingBox.h"

//Forward reference to the OBBHierarchy
class OBBHierarchy;

//TerrainQuad is a Node, which can contain child nodes
struct TerrainQuad
{
    TerrainQuad()
    {
        m_vMin = toXMVECTOR(Infinity, Infinity, Infinity, 1);
        m_vMax = toXMVECTOR(-Infinity, -Infinity, -Infinity, 1);
        m_nextChildNode = nullptr;
    }

    TerrainQuad(int rowStart, int rowEnd, int colStart, int colEnd, int width, XCVecIntrinsic4 min, XCVecIntrinsic4 max)
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

    OBBHierarchy*                               getChildNode() { return m_nextChildNode.get(); }
    void                                        setChildNodeOBB(std::unique_ptr<OBBHierarchy> obb) { m_nextChildNode = std::move(obb); }
    bool                                        hasChildNode();


    std::unique_ptr<OrientedBoundingBox>        m_bbox;
    std::unique_ptr<OBBHierarchy>               m_nextChildNode;

    int                                         m_totalWidth;
    int                                         m_rowStart;
    int                                         m_rowEnd;
    int                                         m_colStart;
    int                                         m_colEnd;

    XCVecIntrinsic4                                    m_vMin;
    XCVecIntrinsic4                                    m_vMax;
};

class OBBHierarchy
{
public:
    static const int                            NO_OF_QUADS_EACH_LEVEL = 4;

    OBBHierarchy();
    ~OBBHierarchy();

    void                                        addQuad(std::unique_ptr<TerrainQuad> bbox) { m_Quads.push_back(std::move(bbox));  }

    void                                        createTerrainOBBHierarchy(int levels, int rowStart, int totalRows, int colStart, int totalColumns, int totalWidth);

    void                                        computeQuad(int row, int col, XCVec3 pos);
    void                                        computeOBBForAllQuads();

    void                                        Update(float dt);
    void                                        Draw(RenderContext& context);

    TerrainQuad*                                getQuadCollidingWithOBB(OrientedBoundingBox* bbox);

    void                                        Transform(XCMatrix4 translateMat, XCMatrix4 rotateMatrix);

private:
    std::vector<std::unique_ptr<TerrainQuad>>   m_Quads;
};