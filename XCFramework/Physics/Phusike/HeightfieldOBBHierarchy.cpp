/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "PhysicsPrecompiledHeader.h"

#include "HeightfieldOBBHierarchy.h"

#include "Assets/Packages/Schema/ShaderTypes_generated.h"

#include "Phusike/OBBBoundVolume.h"

#include "Graphics/BasicGeometry/RenderableOBB.h"
#include "Graphics/VertexBuffer.h"

HeightfieldOBBHierarchy::HeightfieldOBBHierarchy(const PhysicsDesc::PhysicsHeightFieldDesc& heightfieldDesc)
    : m_rows(heightfieldDesc.m_rows)
    , m_cols(heightfieldDesc.m_cols)
    , m_vertexBuffer(heightfieldDesc.m_vertexBuffer)
    , m_vertexFormat(heightfieldDesc.m_vertexFormat)
{
}

HeightfieldOBBHierarchy::~HeightfieldOBBHierarchy()
{
    XCDELETE(m_heightfieldQuadTree);
}

void HeightfieldQuad::Update(f32 dt)
{
    m_bbox->Update(dt);

    //Check further in its child nodes
    for (auto& childNode : GetNodesMutable())
    {
        childNode->GetMutable()->Update(dt);
    }
}

void HeightfieldOBBHierarchy::Update(f32 dt)
{
    HeightfieldQuad* rootNode = m_heightfieldQuadTree->GetRootNode();
    rootNode->GetMutable()->Update(dt);
}

void HeightfieldQuad::Transform(const XCVec4& translate, const XCVec4& orientation)
{
    static_cast<OBBBoundVolume*>(m_bbox)->Transform(translate, orientation);

    //Check further in its child nodes
    for (auto& childNode : GetNodesMutable())
    {
        childNode->GetMutable()->Transform(translate, orientation);
    }
}

void HeightfieldOBBHierarchy::Transform(const XCVec4& translate, const XCVec4& orientation)
{
    HeightfieldQuad* rootNode = m_heightfieldQuadTree->GetRootNode();
    rootNode->GetMutable()->Transform(translate, orientation);
}

void HeightfieldOBBHierarchy::CreateHeightfieldOBBHierarchy()
{
    u32 rowStart = 0;
    u32 totalRows = m_rows;
    u32 colStart = 0;
    u32 totalColumns = m_cols;
    u32 totalWidth = m_cols;

    static const u32 noOfQuads = 4;

    XCVec4 vMin(Infinity, Infinity, Infinity, 1);
    XCVec4 vMax(-Infinity, -Infinity, -Infinity, 1);

    //Setup of quadruples, which divides the terrain into smaller quads. Conduct object test on the quads and moving towards the most inner quad within the OBBHierarchy
    //First create root bound box
    HeightfieldQuad* rootQuad = XCNEW(HeightfieldQuad)(rowStart, totalRows, colStart, totalColumns, totalColumns, vMin, vMax);
    m_heightfieldQuadTree = XCNEW(XCTree<HeightfieldQuad*>)(rootQuad);

    //1st degree Child nodes
    HeightfieldQuad* quad1     = XCNEW(HeightfieldQuad)(0, totalRows / (noOfQuads / 2), 0, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    HeightfieldQuad* quadNode1 = m_heightfieldQuadTree->AddNode(quad1);

    HeightfieldQuad* quad11 = XCNEW(HeightfieldQuad)(0, (totalRows / (noOfQuads / 2)) / 2, 0, (totalColumns / (noOfQuads / 2)) / 2, totalColumns, vMin, vMax);
    HeightfieldQuad* quad22 = XCNEW(HeightfieldQuad)(0, (totalRows / (noOfQuads / 2)) / 2, (totalColumns / (noOfQuads / 2)) / 2, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    HeightfieldQuad* quad33 = XCNEW(HeightfieldQuad)(totalRows / (noOfQuads / 2) / 2, totalRows / (noOfQuads / 2), 0, (totalColumns / (noOfQuads / 2)) / 2, totalColumns, vMin, vMax);
    HeightfieldQuad* quad44 = XCNEW(HeightfieldQuad)(totalRows / (noOfQuads / 2) / 2, totalRows / (noOfQuads / 2), totalColumns / (noOfQuads / 2) / 2, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);

    m_heightfieldQuadTree->AddNode(quadNode1, quad11);
    m_heightfieldQuadTree->AddNode(quadNode1, quad22);
    m_heightfieldQuadTree->AddNode(quadNode1, quad33);
    m_heightfieldQuadTree->AddNode(quadNode1, quad44);

    HeightfieldQuad* quad2 = XCNEW(HeightfieldQuad)(0, totalRows / (noOfQuads / 2), totalColumns / (noOfQuads / 2), totalColumns, totalColumns, vMin, vMax);
    HeightfieldQuad* quad3 = XCNEW(HeightfieldQuad)(totalRows / (noOfQuads / 2), totalRows, 0, totalColumns / (noOfQuads / 2), totalColumns, vMin, vMax);
    HeightfieldQuad* quad4 = XCNEW(HeightfieldQuad)(totalRows / (noOfQuads / 2), totalRows, totalColumns / (noOfQuads / 2), totalColumns, totalColumns, vMin, vMax);

    m_heightfieldQuadTree->AddNode(quad2);
    m_heightfieldQuadTree->AddNode(quad3);
    m_heightfieldQuadTree->AddNode(quad4);

    ComputeQuads();
}

void HeightfieldOBBHierarchy::CreateHeightfieldOBBHierarchy(u32 levels)
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

    std::unique_ptr<HeightfieldQuad> quad;

    //For each level, we will have a OBBH, so create it here
    //Before that create noOfQuadsEachLevel this level
    //Also here we will loop through the current m_quads and divide them further.

    //m_Quads.back()->setChildNodeOBB(childNodeOBB);

    for(u32 quadIndex = 0; quadIndex < m_Quads.size(); quadIndex++)
    {
        std::unique_ptr<HeightfieldOBBHierarchy> childNodeOBB = std::make_unique<HeightfieldOBBHierarchy>();

        for (i32 quadSection = 0; quadSection < NO_OF_QUADS_EACH_LEVEL; quadSection++)
        {

            switch (quadSection)
            {
            case 0:
                quad = std::make_unique<HeightfieldQuad>(rowStart, rowMid, colStart, colMid, totalWidth, vMin, vMax);
                break;
            case 1:
                quad = std::make_unique<HeightfieldQuad>(rowStart, rowMid, colMid, totalColumns, totalWidth, vMin, vMax);
                break;
            case 2:
                quad = std::make_unique<HeightfieldQuad>(rowMid, totalRows, colStart, colMid, totalWidth, vMin, vMax);
                break;
            case 3:
                quad = std::make_unique<HeightfieldQuad>(rowMid, totalRows, colMid, totalColumns, totalWidth, vMin, vMax);
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
            m_Quads[quadIndex]->GetChildNode()->CreateHeightfieldOBBHierarchy(levels - 1, rowStart, rowMid - rowStart, colStart, colMid - colStart, totalWidth);
        }
    }
#endif
}

bool HeightfieldQuad::ComputeQuad(u32 row, u32 col, XCVec4& pos)
{
    bool retVal = false;

    if (row >= m_rowStart && row < m_rowEnd && col >= m_colStart && col < m_colEnd)
    {
        //Found the respective quad. Insert into the quad data and return
        m_vMin = VectorMin(m_vMin, pos);
        m_vMax = VectorMax(m_vMax, pos);

        retVal = true;
    }

    //Check further in its child nodes
    for (auto& childNode : GetNodesMutable())
    {
        childNode->GetMutable()->ComputeQuad(row, col, pos);
    }

    return retVal;
}

void HeightfieldOBBHierarchy::ComputeQuads()
{
    HeightfieldQuad* rootNode = m_heightfieldQuadTree->GetRootNode();

    switch(m_vertexFormat)
    {
        case VertexFormat_PositionNormalTexture:
            {
                VertexBuffer<VertexPosNormTex>* vertexBuffer = static_cast<VertexBuffer<VertexPosNormTex>*>(m_vertexBuffer);

                u32 vertCount = 0;
                for(u32 rowIndex = 0; rowIndex < m_rows; rowIndex++)
                {
                    for(u32 colIndex = 0; colIndex < m_cols; colIndex++)
                    {
                        const VertexPosNormTex& vertex = vertexBuffer->m_vertexData[vertCount++];
                        rootNode->GetMutable()->ComputeQuad(rowIndex, colIndex, XCVec4(vertex.Pos));
                    }
                }

                ComputeOBBForAllQuads();
                break;
            }

        default:
            XCASSERTMSG(false, "[HeightfieldOBBHierarchy] Unknow format specified");
    }
}

void HeightfieldQuad::ComputeOBBForAllQuads()
{
    m_bbox = XCNEW(OBBBoundVolume)(PhysicsDesc::PhysicsBoundBoxDesc(m_vMin, m_vMax));

    //Compute on child nodes
    for (auto& list : GetNodesMutable())
    {
        list->GetMutable()->ComputeOBBForAllQuads();
    }
}

void HeightfieldOBBHierarchy::ComputeOBBForAllQuads()
{
    HeightfieldQuad* rootNode = m_heightfieldQuadTree->GetRootNode();
    rootNode->GetMutable()->ComputeOBBForAllQuads();
}

const HeightfieldQuad* HeightfieldQuad::GetQuadCollidingWithOBB(const OBBBoundVolume* bbox) const
{
    DirectX::ContainmentType type = static_cast<OBBBoundVolume*>(m_bbox)->Contains(bbox);
    if (type == DirectX::CONTAINS || type == DirectX::INTERSECTS)
    {
        //If Collided, check if we have child node, then return from that child HeightfieldQuad
        return this;
    }

    //Compute on child nodes
    for (auto& list : GetNodes())
    {
        return list->GetMutable()->GetQuadCollidingWithOBB(bbox);
    }

    //No collision detected, OBB must be outside the quad
    return nullptr;
}

//Recursive Algorithm which finds the most inner quad that collides with the OBB
const HeightfieldQuad* HeightfieldOBBHierarchy::GetQuadCollidingWithOBB(const OBBBoundVolume* bbox) const
{
    //Check if colliding with current quads
    HeightfieldQuad* rootNode = m_heightfieldQuadTree->GetRootNode();
    return rootNode->Get()->GetQuadCollidingWithOBB(bbox);
}

XCVec4 HeightfieldOBBHierarchy::GetPointOfContactWithOBB(const IPhysicsBoundVolume* bboxActor) const
{
    const OBBBoundVolume* obb = static_cast<const OBBBoundVolume*>(bboxActor);
    const HeightfieldQuad* quad = GetQuadCollidingWithOBB(obb);
    XCVec4 vertexPos;

    if(quad != nullptr)
    {
        VertexBuffer<VertexPosNormTex>* vertexBuffer = static_cast<VertexBuffer<VertexPosNormTex>*>(m_vertexBuffer);

        //Find the vertex within the terrain quad
        for(u32 rowIndex = quad->m_rowStart; rowIndex < quad->m_rowEnd; rowIndex++)
        {
            for(u32 colIndex = quad->m_colStart; colIndex < quad->m_colEnd; colIndex++)
            {
                vertexPos = XCVec4(vertexBuffer->m_vertexData[quad->m_totalWidth * rowIndex + colIndex].Pos);

                DirectX::ContainmentType contain = obb->Contains(vertexPos);
                if(contain == DirectX::CONTAINS || contain == DirectX::INTERSECTS)
                {
                    return vertexPos;
                }
            }
        }
    }

    //If we come here that means no collision with any terrain vertex. So return nothing
    return XCVec4();
}

void HeightfieldOBBHierarchy::GetAllOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const
{
    m_heightfieldQuadTree->GetRootNode()->GetOBBInfo(outInfo);
}

void HeightfieldQuad::GetOBBInfo(std::vector<PhysicsPlayground::OBBInfo>& outInfo) const
{
    m_bbox->GetOBBInfo(outInfo);

    //Get info from all child nodes
    for(auto& list : GetNodes())
    {
        return list->Get()->GetOBBInfo(outInfo);
    }
}
