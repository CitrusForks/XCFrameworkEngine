/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "TerrainOBBHierarchy.h"
#include "Graphics/BasicGeometry/RenderableOBB.h"

TerrainOBBHierarchy::TerrainOBBHierarchy()
{
}

TerrainOBBHierarchy::~TerrainOBBHierarchy()
{
    XCDELETE(m_terrainQuadTree);
}

void TerrainQuad::Update(f32 dt)
{
    m_bbox->Update(dt);
}

void TerrainOBBHierarchy::Update(f32 dt)
{
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    Update(rootNode, dt);
}

void TerrainOBBHierarchy::Update(XCTreeNode<TerrainQuad*>& node, f32 dt)
{
    node.Get()->Update(dt);

    //Check further in its child nodes
    for (auto& childNode : node.GetNodesMutable())
    {
        Update(*childNode, dt);
    }
}

void TerrainQuad::Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix)
{
    m_bbox->Transform(translateMat, rotateMatrix);
}

void TerrainOBBHierarchy::Transform(XCMatrix4& translateMat, XCMatrix4& rotateMatrix)
{
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    Transform(rootNode, translateMat, rotateMatrix);
}

void TerrainOBBHierarchy::Transform(XCTreeNode<TerrainQuad*>& node, XCMatrix4& translateMat, XCMatrix4& rotateMatrix)
{
    node.Get()->Transform(translateMat, rotateMatrix);

    //Check further in its child nodes
    for (auto& childNode : node.GetNodesMutable())
    {
        Transform(*childNode, translateMat, rotateMatrix);
    }
}

void TerrainQuad::Draw(RenderContext& context)
{
    m_bbox->Draw(context);
}

void TerrainOBBHierarchy::Draw(RenderContext& context)
{
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    Draw(rootNode, context);
}

void TerrainOBBHierarchy::Draw(XCTreeNode<TerrainQuad*>& node, RenderContext& context)
{
    node.Get()->Draw(context);

    //Check further in its child nodes
    for (auto& childNode : node.GetNodesMutable())
    {
        Draw(*childNode, context);
    }
}

void TerrainOBBHierarchy::CreateTerrainOBBHierarchy(i32 rowStart, i32 totalRows, i32 colStart, i32 totalColumns, i32 totalWidth)
{
    i32 noOfQuads = 4;

    XCVec4 vMin(Infinity, Infinity, Infinity, 1);
    XCVec4 vMax(-Infinity, -Infinity, -Infinity, 1);

    //Setup of quadruples, which divides the terrain into smaller quads. Conduct object test on the quads and moving towards the most inner quad within the OBBHierarchy
    //First create root bound box
    TerrainQuad* rootQuad = XCNEW(TerrainQuad)(rowStart, totalRows, colStart, totalColumns, totalColumns, vMin, vMax);
    m_terrainQuadTree = XCNEW(XCTree<TerrainQuad*>)(rootQuad);

    //1st degree Child nodes
    TerrainQuad* quad1 = XCNEW(TerrainQuad)(0, totalRows / (noOfQuads / 2), 0, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    XCTreeNode<TerrainQuad*>& quadNode1 = m_terrainQuadTree->AddNode(quad1);

    TerrainQuad* quad11 = XCNEW(TerrainQuad)(0, (totalRows / (noOfQuads / 2)) / 2, 0, (totalColumns / (noOfQuads / 2)) / 2, totalColumns, vMin, vMax);
    TerrainQuad* quad22 = XCNEW(TerrainQuad)(0, (totalRows / (noOfQuads / 2)) / 2, (totalColumns / (noOfQuads / 2)) / 2, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    TerrainQuad* quad33 = XCNEW(TerrainQuad)(totalRows / (noOfQuads / 2) / 2, totalRows / (noOfQuads / 2), 0, (totalColumns / (noOfQuads / 2)) / 2, totalColumns, vMin, vMax);
    TerrainQuad* quad44 = XCNEW(TerrainQuad)(totalRows / (noOfQuads / 2) / 2, totalRows / (noOfQuads / 2), totalColumns / (noOfQuads / 2) / 2, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);

    m_terrainQuadTree->AddNode(quadNode1, quad11);
    m_terrainQuadTree->AddNode(quadNode1, quad22);
    m_terrainQuadTree->AddNode(quadNode1, quad33);
    m_terrainQuadTree->AddNode(quadNode1, quad44);

    TerrainQuad* quad2 = XCNEW(TerrainQuad)(0, totalRows / (noOfQuads / 2), totalColumns / (noOfQuads / 2), totalColumns, totalColumns, vMin, vMax);
    TerrainQuad* quad3 = XCNEW(TerrainQuad)(totalRows / (noOfQuads / 2), totalRows, 0, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    TerrainQuad* quad4 = XCNEW(TerrainQuad)(totalRows / (noOfQuads / 2), totalRows, totalColumns / (noOfQuads / 2), totalColumns, totalColumns, vMin, vMax);

    m_terrainQuadTree->AddNode(quad2);
    m_terrainQuadTree->AddNode(quad3);
    m_terrainQuadTree->AddNode(quad4);
}

void TerrainOBBHierarchy::CreateTerrainOBBHierarchy(i32 levels, i32 rowStart, i32 totalRows, i32 colStart, i32 totalColumns, i32 totalWidth)
{
#if defined(UNUSED)
    //Base condition to return from here, if this is the 0th level.
    if (levels == 0)
    {
        return;
    }

    i32 noOfDividedRows = NO_OF_QUADS_EACH_LEVEL / 2;
    i32 noOfDividedCols = NO_OF_QUADS_EACH_LEVEL / 2;

    i32 rowMid = totalRows / noOfDividedRows;
    i32 colMid = totalColumns / noOfDividedCols;


    XCVec4 vMin(Infinity, Infinity, Infinity, 1);
    XCVec4 vMax(-Infinity, -Infinity, -Infinity, 1);

    std::unique_ptr<TerrainQuad> quad;

    //For each level, we will have a OBBH, so create it here
    //Before that create noOfQuadsEachLevel this level
    //Also here we will loop through the current m_quads and divide them further.

    //m_Quads.back()->setChildNodeOBB(childNodeOBB);

    for(u32 quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        std::unique_ptr<TerrainOBBHierarchy> childNodeOBB = std::make_unique<TerrainOBBHierarchy>();

        for (i32 quadSection = 0; quadSection < NO_OF_QUADS_EACH_LEVEL; quadSection++)
        {

            switch (quadSection)
            {
            case 0:
                quad = std::make_unique<TerrainQuad>(rowStart, rowMid, colStart, colMid, totalWidth, vMin, vMax);
                break;
            case 1:
                quad = std::make_unique<TerrainQuad>(rowStart, rowMid, colMid, totalColumns, totalWidth, vMin, vMax);
                break;
            case 2:
                quad = std::make_unique<TerrainQuad>(rowMid, totalRows, colStart, colMid, totalWidth, vMin, vMax);
                break;
            case 3:
                quad = std::make_unique<TerrainQuad>(rowMid, totalRows, colMid, totalColumns, totalWidth, vMin, vMax);
                break;
            default:
                break;
            }

            m_Quads[quadIndex]->SetChildNodeOBB(std::move(childNodeOBB));
            m_Quads[quadIndex]->GetChildNode()->AddQuad(std::move(quad));
        }
    }
    
    //Now see if there exists further level > 1
    if (levels > 1)
    {
        //Then call respectively for every quad create
        for(u32 quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
        {
            m_Quads[quadIndex]->GetChildNode()->CreateTerrainOBBHierarchy(levels - 1, rowStart, rowMid - rowStart, colStart, colMid - colStart, totalWidth);
        }
    }
#endif
}

bool TerrainQuad::ComputeQuad(i32 row, i32 col, XCVec4& pos)
{
    bool retVal = false;

    if (row >= m_rowStart && row < m_rowEnd && col >= m_colStart && col < m_colEnd)
    {
        //Found the respective quad. Insert into the quad data and return
        m_vMin = VectorMin(m_vMin, pos);
        m_vMax = VectorMax(m_vMax, pos);

        retVal = true;
    }

    return retVal;
}
void TerrainOBBHierarchy::ComputeQuad(i32 row, i32 col, XCVec4& pos)
{
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    ComputeQuad(rootNode, row, col, pos);
}

void TerrainOBBHierarchy::ComputeQuad(XCTreeNode<TerrainQuad*>& node, i32 row, i32 col, XCVec4& pos)
{
    node.GetMutable()->ComputeQuad(row, col, pos);

    //Check further in its child nodes
    for (auto& childNode : node.GetNodesMutable())
    {
        ComputeQuad(*childNode, row, col, pos);
    }
}

void TerrainQuad::ComputeOBBForAllQuads()
{
    m_bbox = std::make_unique<RenderableOBB>();
    m_bbox->Init();
    m_bbox->CreateBoundBox(m_vMin, m_vMax);
}

void TerrainOBBHierarchy::ComputeOBBForAllQuads()
{
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    ComputeOBBForAllQuads(rootNode);
}

void TerrainOBBHierarchy::ComputeOBBForAllQuads(XCTreeNode<TerrainQuad*>& node)
{
    node.GetMutable()->ComputeOBBForAllQuads();

    //Compute on child nodes
    for (auto& list : node.GetNodesMutable())
    {
        ComputeOBBForAllQuads(*list);
    }
}

TerrainQuad* TerrainQuad::GetQuadCollidingWithOBB(OrientedBoundingBox* bbox)
{
    DirectX::ContainmentType type = m_bbox->m_TransformedBox.Contains(bbox->m_TransformedBox);
    if (type == DirectX::CONTAINS || type == DirectX::INTERSECTS)
    {
        //If Collided, check if we have child node, then return from that child TerrainQuad
        return this;
    }

    //No collision detected, OBB must be outside the quad
    return nullptr;
}

//Recursive Algorithm which finds the most inner quad that collides with the OBB
TerrainQuad* TerrainOBBHierarchy::GetQuadCollidingWithOBB(OrientedBoundingBox* bbox)
{
    //Check if colliding with current quads
    XCTreeNode<TerrainQuad*>& rootNode = m_terrainQuadTree->GetRootNode();
    return GetQuadCollidingWithOBB(rootNode, bbox);
}

TerrainQuad* TerrainOBBHierarchy::GetQuadCollidingWithOBB(XCTreeNode<TerrainQuad*>& node, OrientedBoundingBox* bbox)
{
    TerrainQuad* outResult = node.GetMutable()->GetQuadCollidingWithOBB(bbox);

    if (outResult == nullptr)
    {
        //Compute on child nodes
        for (auto& list : node.GetNodesMutable())
        {
            outResult = GetQuadCollidingWithOBB(*list, bbox);

            if (outResult)
            {
                break;
            }
        }
    }

    return outResult;
}