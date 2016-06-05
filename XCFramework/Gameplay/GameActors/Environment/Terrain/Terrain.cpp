/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#if defined(WIN32)
#include <wingdi.h>
#endif

#include "Terrain.h"

#include "Gameplay/WorldEventTypes.h"

#include "Graphics/XC_Graphics.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"
#include "Gameplay/XC_Camera/XC_CameraManager.h"
#include "Graphics/XC_Lighting/XC_LightManager.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Event/EventBroadcaster.h"

Terrain::Terrain()
{
    m_material.Ambient = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.Diffuse = XCVec4(0.48f, 0.77f, 0.46f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = ShaderType_TerrainMultiTexture;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TERRAIN;

    m_textures.clear();
}

Terrain::~Terrain(void)
{
    if(m_pBitmapImage)
        delete(m_pBitmapImage);
}

void Terrain::PreLoad(const void* fbBuffer)
{
    const FBMultiTexturedTerrain* fbTerrainBuff = (FBMultiTexturedTerrain*)fbBuffer;
    m_pHeightMapFileName = getPlatformPath(fbTerrainBuff->BitmapFileName()->c_str());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName1()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName2()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DBlendResourceName3()->c_str()));

    m_initialPosition = XCVec3(fbTerrainBuff->Position()->x(), fbTerrainBuff->Position()->y(), fbTerrainBuff->Position()->z());
    m_currentPosition = XMLoadFloat3(&m_initialPosition);
    
    m_rows = fbTerrainBuff->Rows();
    m_cols = fbTerrainBuff->Column();
    m_rowSpacing = (float)fbTerrainBuff->RowSpacing();
    m_colSpacing = (float)fbTerrainBuff->ColSpacing();

    ComputeVertices();

    PhysicsActor::PreLoad(fbBuffer);

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(PerObjectBuffer)));
#endif
}

//Multi Textured Terrain
void Terrain::PreLoad(const char* _pHeightMapFileName, 
    std::string terrainTexture,
    std::string terrainTexture1,
    std::string terrainTexture2,
    std::string blendMap,
    XCVec3 initialPosition, 
    int _rows, 
    int _column, 
    float _rowSpacing, 
    float _colSpacing)
{
    m_pHeightMapFileName = getPlatformPath(_pHeightMapFileName);

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_textures.push_back(&resMgr.AcquireResource(terrainTexture.c_str()));
    m_textures.push_back(&resMgr.AcquireResource(terrainTexture1.c_str()));
    m_textures.push_back(&resMgr.AcquireResource(terrainTexture2.c_str()));
    m_textures.push_back(&resMgr.AcquireResource(blendMap.c_str()));
    
    m_currentPosition = XMLoadFloat3(&initialPosition);
    m_rows = _rows;
    m_cols = _column;
    m_rowSpacing = _rowSpacing;
    m_colSpacing = _colSpacing;

    ComputeVertices();

#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(PerObjectBuffer)));
#endif
}

void Terrain::Load()
{
    LoadHeightMap();
    GenerateVertices();
    GenerateIndicesWithTextureMapping();
    GenerateVerticesNormal();
    BuildGeometryBuffer();
    UnloadHeightMap();

    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(XMLoadFloat3(&m_initialPosition), XMLoadFloat3(&vec), XMLoadFloat3(&vec), 1000, (float)0.2); //Immovable
    
    //Terrain is loaded, so fire up the world ready event
    Event_World event(EventType_WorldReady);
    EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadcaster.BroadcastEvent(&event);

    IActor::Load();
}

void Terrain::UpdateState()
{
    for (auto& tex : m_textures)
    {
        if (!tex->m_Resource->IsLoaded())
        {
            return;
        }
    }

    m_actorState = IActor::ActorState_Loaded;
    IActor::UpdateState();
}

void Terrain::GenerateVertices()
{
    //Set the position of every vertex
    m_vertexPosNormTexBuffer.m_vertexData.resize(m_totalVertices);

    int verticesIndex = 0;
    int bitmapRGBIndex = 0;

    XCVecIntrinsic4 vMin = toXMVECTOR(Infinity, Infinity, Infinity, 1);
    XCVecIntrinsic4 vMax = toXMVECTOR(-Infinity, -Infinity, -Infinity, 1);
    
    int noOfQuads = 4;
    
    //Setup of quadruples, which divides the terrain into smaller quads. Conduct object test on the quads and moving towards the most inner quad within the OBBHierarchy
    //First create root bound box
    m_OBBHierarchy = std::make_unique<OBBHierarchy>();
    std::unique_ptr<TerrainQuad> rootQuad = std::make_unique<TerrainQuad>(0, m_rows, 0, m_cols, m_cols, vMin, vMax);
    //m_OBBHierarchy->createTerrainOBBHierarchy(2, 0, m_rows, 0, m_cols, m_cols);
    
    
    //1st degree Child nodes
    std::unique_ptr<OBBHierarchy> childNode = std::make_unique<OBBHierarchy>();
    std::unique_ptr<TerrainQuad> quad1 = std::make_unique<TerrainQuad>(0, m_rows / (noOfQuads / 2), 0, m_cols / (noOfQuads / 2), m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad2 = std::make_unique<TerrainQuad>(0, m_rows / (noOfQuads / 2), m_cols / (noOfQuads / 2), m_cols, m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad3 = std::make_unique<TerrainQuad>(m_rows / (noOfQuads / 2), m_rows, 0, m_cols / (noOfQuads / 2), m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad4 = std::make_unique<TerrainQuad>(m_rows / (noOfQuads / 2), m_rows, m_cols / (noOfQuads / 2), m_cols, m_cols, vMin, vMax);
    
    
    //This sub division will be a node to quad1
    std::unique_ptr<OBBHierarchy> childNode1 = std::make_unique<OBBHierarchy>();
    std::unique_ptr<TerrainQuad> quad11 = std::make_unique<TerrainQuad>(0, (m_rows / (noOfQuads / 2)) / 2, 0, (m_cols / (noOfQuads / 2)) / 2, m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad22 = std::make_unique<TerrainQuad>(0, (m_rows / (noOfQuads / 2)) / 2, (m_cols / (noOfQuads / 2)) / 2, m_cols / (noOfQuads / 2), m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad33 = std::make_unique<TerrainQuad>(m_rows / (noOfQuads / 2) / 2, m_rows / (noOfQuads / 2), 0, (m_cols / (noOfQuads / 2)) / 2, m_cols, vMin, vMax);
    std::unique_ptr<TerrainQuad> quad44 = std::make_unique<TerrainQuad>(m_rows / (noOfQuads / 2) / 2, m_rows / (noOfQuads / 2), m_cols / (noOfQuads / 2) / 2, m_cols / (noOfQuads / 2), m_cols, vMin, vMax);
    
    childNode1->AddQuad(std::move(quad11));
    childNode1->AddQuad(std::move(quad22));
    childNode1->AddQuad(std::move(quad33));
    childNode1->AddQuad(std::move(quad44));
    
    quad1->SetChildNodeOBB(std::move(childNode1));
    
    childNode->AddQuad(std::move(quad1));
    childNode->AddQuad(std::move(quad2));
    childNode->AddQuad(std::move(quad3));
    childNode->AddQuad(std::move(quad4));
    
    rootQuad->SetChildNodeOBB(std::move(childNode));
    m_OBBHierarchy->AddQuad(std::move(rootQuad));


    for (int rowIndex = 0; rowIndex < m_rows; rowIndex++)
    {
        for (int colIndex = 0; colIndex < m_cols; colIndex++)
        {
            float x = m_initialPosition.x - (colIndex * m_rowSpacing);
            float z = m_initialPosition.z + (rowIndex * m_colSpacing);
      
            //float y = m_initialPosition.y + (float)GetHeightAt( (m_rows * rowIndex) + colIndex)/15.0f;
            //float y = m_initialPosition.y + ((float) GetHeightAt(k) / 10.0f);
            float y = (float) GetHeightAt(bitmapRGBIndex) / 50.0f;

            //Necessary increment by 3, as the bitmap contains rgb format at each pixel, thus increment by 3.
            bitmapRGBIndex += 3;

            switch(m_useShaderType)
            {
                case ShaderType_LightTexture:
                case ShaderType_TerrainMultiTexture:
                    {
                        XCVec3 pos(x, y, z);
                        m_vertexPosNormTexBuffer.m_vertexData[verticesIndex++] = VertexPosNormTex(XCVec3Unaligned(x, y, z), XCVec3Unaligned(0.0f, 1.0f, 0.0f), XCVec2Unaligned(0.0f, 1.0f));
                        m_OBBHierarchy->ComputeQuad(rowIndex, colIndex, pos);
                        break;
                    }
            }
        }
    }
    m_OBBHierarchy->ComputeOBBForAllQuads();
}

void Terrain::GenerateVerticesNormal()
{
    //Traverse through the vertices

    for(unsigned int vertexIndex = 0; vertexIndex < m_indexBuffer.m_indexData.size() - 3; vertexIndex += 3)
    {
        XCVecIntrinsic4 v1 = XMLoadFloat3(ToXCVec3(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex]].Pos));
        XCVecIntrinsic4 v2 = XMLoadFloat3(ToXCVec3(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 1]].Pos));
        XCVecIntrinsic4 v3 = XMLoadFloat3(ToXCVec3(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 2]].Pos));

        XCVecIntrinsic4 vertexNormal = GetNormalFromPoints(v1, v2, v3);
        XMStoreFloat3(&m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex]].Norm, vertexNormal);

        vertexNormal = GetNormalFromPoints(v2, v1, v3);
        XMStoreFloat3(&m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 1]].Norm, vertexNormal);

        vertexNormal = GetNormalFromPoints(v3, v1, v2);
        XMStoreFloat3(&m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 2]].Norm, vertexNormal);
    }
}

XCVecIntrinsic4 Terrain::CheckTerrainCollisionFromPoint(OrientedBoundingBox* bbox)
{
    TerrainQuad* quad = m_OBBHierarchy->GetQuadCollidingWithOBB(bbox);
    XCVecIntrinsic4 vertexPos = XMVectorZero();

    if (quad != nullptr)
    {
        //Find the vertex within the terrain quad
        for (int rowIndex = quad->m_rowStart; rowIndex < quad->m_rowEnd; rowIndex++)
        {
            for (int colIndex = quad->m_colStart; colIndex < quad->m_colEnd; colIndex++)
            {
                vertexPos = XMLoadFloat3(ToXCVec3(m_vertexPosNormTexBuffer.m_vertexData[quad->m_totalWidth * rowIndex + colIndex].Pos));

                ContainmentType contain = bbox->m_TransformedBox.Contains(vertexPos);
                if (contain == CONTAINS || contain == INTERSECTS)
                {
                    return vertexPos;
                }
            }
        }
    }

    //If we come here that means no collision with any terrain vertex. So return nothing
    return XMVectorZero();
}


void Terrain::LoadHeightMap()
{
    FILE *pFilePtr;
    int error;

    unsigned int count;

    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;

    //Open the height map file in binary
    error = fopen_s(&pFilePtr, m_pHeightMapFileName.c_str(), "rb");
    if (error != 0)
    {
        SimpleTerrain::GenerateVertices();
        XCASSERT(false);
        return;
    }

    //Read the file header
    count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, pFilePtr);
    if (count != 1)
    {
        SimpleTerrain::GenerateVertices();
        XCASSERT(false);
        return;
    }

    //Read in the bitmap info header
    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFilePtr);
    if (count != 1)
    {
        SimpleTerrain::GenerateVertices();
        XCASSERT(false);
        return;
    }

    m_rows = bitmapInfoHeader.biWidth;
    m_cols = bitmapInfoHeader.biHeight;

    m_totalVertices = m_rows * m_cols;

    int imageSize = m_rows * m_cols * 3;

    m_pBitmapImage = new unsigned char[imageSize];

    //Move to first position within the bitmap file
    fseek(pFilePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //Read in the bitmap image data
    count = fread(m_pBitmapImage, 1, imageSize, pFilePtr);
    if (count != imageSize)
    {
        XCASSERT(false);
        return;
    }

    //Close the file
    fclose(pFilePtr);
}

void Terrain::UnloadHeightMap()
{
    if (m_pBitmapImage)
    {
        delete(m_pBitmapImage);
        m_pBitmapImage = nullptr;
    }
}

char Terrain::GetHeightAt(int _index) const
{
    return m_pBitmapImage[_index];
}

XCVecIntrinsic4 Terrain::GetPointAtIndex(int pointIndex) const
{
    return XMLoadFloat3(ToXCVec3(m_vertexPosNormTexBuffer.m_vertexData[pointIndex].Pos));
}

void Terrain::Update(float dt)
{
    SimpleTerrain::Update(dt);

    m_OBBHierarchy->Transform(XMMatrixIdentity(), XMMatrixIdentity());
    m_OBBHierarchy->Update(dt);
}

void Terrain::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillSolid);

    context.ApplyShader(m_useShaderType);

    XCShaderHandle* shaderHandle = nullptr;

    // Set constants
    ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix())),
        ToXCMatrix4Unaligned(InverseTranspose(m_World)),
        ToXCMatrix4Unaligned(XMMatrixIdentity()),
        m_material
    };

    if (m_useShaderType == ShaderType_LightTexture)
    {
        shaderHandle = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(ShaderType_LightTexture);
#if defined(XCGRAPHICS_DX12)
        memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(PerObjectBuffer));
        shaderHandle->SetConstantBuffer("PerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);

#else
        shaderHandle->setCBPerObject(context.GetDeviceContext(), perObject);
#endif
        shaderHandle->SetResource("gDiffuseMap", context.GetDeviceContext(), m_textures[0]);
    }
    else if (m_useShaderType == ShaderType_TerrainMultiTexture)
    {
        shaderHandle = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(ShaderType_TerrainMultiTexture);
#if defined(XCGRAPHICS_DX12)
        memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(PerObjectBuffer));
        shaderHandle->SetConstantBuffer("PerObjectBuffer", context.GetDeviceContext(), m_pCBPerObject->m_gpuHandle);
#else
        shaderHandle->setCBPerObject(context.GetDeviceContext(), perObject);
#endif
        shaderHandle->SetResource("gDiffuseMap",  context.GetDeviceContext(), m_textures[0]);
        shaderHandle->SetResource("gDiffuseMap1", context.GetDeviceContext(), m_textures[1]);
        shaderHandle->SetResource("gDiffuseMap2", context.GetDeviceContext(), m_textures[2]);
        shaderHandle->SetResource("gBlendMap",    context.GetDeviceContext(), m_textures[3]);
    }

    XC_LightManager* lightMgr = (XC_LightManager*)&SystemLocator::GetInstance()->RequestSystem("LightsManager");
    shaderHandle->SetConstantBuffer("cbLightsPerFrame", context.GetDeviceContext(), lightMgr->getLightConstantBuffer()->m_gpuHandle);

    shaderHandle->SetVertexBuffer(context.GetDeviceContext(), &m_vertexPosNormTexBuffer);
    shaderHandle->SetIndexBuffer(context.GetDeviceContext(), m_indexBuffer);

    context.GetShaderManagerSystem().DrawIndexedInstanced(context.GetDeviceContext(), m_indexBuffer.m_indexData.size(), m_indexBuffer.GetIndexBufferInGPUMem());

    m_OBBHierarchy->Draw(context);
}

void Terrain::Destroy()
{
    SimpleTerrain::Destroy();

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");

    for (auto& tex : m_textures)
    {
        resMgr.ReleaseResource(tex);
    }
    
#if defined(XCGRAPHICS_DX12)
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBPerObject);
#endif
}

XCVec3 Terrain::GetTerrainNormal(float x, float z) const
{
    return XCVec3(0,0,0);
}