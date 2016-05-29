/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "OBBHierarchy.h"

OBBHierarchy::OBBHierarchy()
{
}


OBBHierarchy::~OBBHierarchy()
{
}

void OBBHierarchy::Update(float dt)
{
    for(unsigned int index = 0; index < m_Quads.size(); index++)
    {
        m_Quads[index]->m_bbox->Update(dt);
        if (m_Quads[index]->HasChildNode())
        {
            m_Quads[index]->GetChildNode()->Update(dt);
        }
    }
}

void OBBHierarchy::Transform(XCMatrix4 translateMat, XCMatrix4 rotateMatrix)
{
    for(unsigned int index = 0; index < m_Quads.size(); index++)
    {
        m_Quads[index]->m_bbox->Transform(translateMat, rotateMatrix);

        if (m_Quads[index]->HasChildNode())
        {
            m_Quads[index]->GetChildNode()->Transform(translateMat, rotateMatrix);
        }

    }

}

void OBBHierarchy::Draw(RenderContext& context)
{
    for(unsigned int index = 0; index < m_Quads.size(); index++)
    {
        m_Quads[index]->m_bbox->Draw(context);

        if (m_Quads[index]->HasChildNode())
        {
            m_Quads[index]->GetChildNode()->Draw(context);
        }
    }
}

bool TerrainQuad::HasChildNode()
{ 
    if (m_nextChildNode != nullptr)
    {
        return true;
    }
    return false;
}

void OBBHierarchy::CreateTerrainOBBHierarchy(int levels, int rowStart, int totalRows, int colStart, int totalColumns, int totalWidth)
{
    //Base condition to return from here, if this is the 0th level.
    if (levels == 0)
    {
        return;
    }

    int noOfDividedRows = NO_OF_QUADS_EACH_LEVEL / 2;
    int noOfDividedCols = NO_OF_QUADS_EACH_LEVEL / 2;

    int rowMid = totalRows / noOfDividedRows;
    int colMid = totalColumns / noOfDividedCols;


    XCVecIntrinsic4 vMin = toXMVECTOR(Infinity, Infinity, Infinity, 1);
    XCVecIntrinsic4 vMax = toXMVECTOR(-Infinity, -Infinity, -Infinity, 1);

    std::unique_ptr<TerrainQuad> quad;

        //For each level, we will have a OBBH, so create it here
        //Before that create noOfQuadsEachLevel this level
        //Also here we will loop through the current m_quads and divide them further.
        
        //m_Quads.back()->setChildNodeOBB(childNodeOBB);

    for(unsigned int quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        std::unique_ptr<OBBHierarchy> childNodeOBB = std::make_unique<OBBHierarchy>();

        for (int quadSection = 0; quadSection < NO_OF_QUADS_EACH_LEVEL; quadSection++)
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
        for(unsigned int quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
        {
            m_Quads[quadIndex]->GetChildNode()->CreateTerrainOBBHierarchy(levels - 1, rowStart, rowMid - rowStart, colStart, colMid - colStart, totalWidth);
        }
    }
}

void OBBHierarchy::ComputeQuad(int row, int col, XCVec3 pos)
{
    for(unsigned int quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        if (row >= m_Quads[quadIndex]->m_rowStart && row < m_Quads[quadIndex]->m_rowEnd && col >= m_Quads[quadIndex]->m_colStart && col < m_Quads[quadIndex]->m_colEnd)
        {
            //Found the respective quad. Insert into the quad data and return
            m_Quads[quadIndex]->m_vMin = XMVectorMin(m_Quads[quadIndex]->m_vMin, XMLoadFloat3(&pos));
            m_Quads[quadIndex]->m_vMax = XMVectorMax(m_Quads[quadIndex]->m_vMax, XMLoadFloat3(&pos));

            if (m_Quads[quadIndex]->HasChildNode())
            {
                m_Quads[quadIndex]->GetChildNode()->ComputeQuad(row, col, pos);
            }
            break;
        }
    }
}

void OBBHierarchy::ComputeOBBForAllQuads()
{
    for(unsigned int quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        m_Quads[quadIndex]->m_bbox = std::make_unique<RenderableOBB>();
        m_Quads[quadIndex]->m_bbox->Init();
        m_Quads[quadIndex]->m_bbox->CreateBoundBox(m_Quads[quadIndex]->m_vMin, m_Quads[quadIndex]->m_vMax);

        if (m_Quads[quadIndex]->HasChildNode())
        {
            m_Quads[quadIndex]->GetChildNode()->ComputeOBBForAllQuads();
        }
    }
}

//Recursive Algorithm which finds the most inner quad that collides with the OBB
TerrainQuad* OBBHierarchy::GetQuadCollidingWithOBB(OrientedBoundingBox* bbox)
{
    //Check if colliding with current quads
    for(unsigned int quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        ContainmentType type = m_Quads[quadIndex]->m_bbox->m_TransformedBox.Contains(bbox->m_TransformedBox);
        if (type == CONTAINS || type == INTERSECTS)
        {
            //If Collided, check if we have child node, then return from that child TerrainQuad
            if (m_Quads[quadIndex]->HasChildNode())
            {
                return m_Quads[quadIndex]->GetChildNode()->GetQuadCollidingWithOBB(bbox);
            }
            
            //else return the current TerrainQuad
            return m_Quads[quadIndex].get();
        }
    }

    //Logger("[PHYSICS] Did not detect collision with quad : %d\n", m_Quads.size());

    //No collision detected, OBB must be outside the quad
    return nullptr;
}