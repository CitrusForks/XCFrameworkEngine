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
#include "TerrainOBBHierarchy.h"

#include "Gameplay/XCCamera/XCCameraManager.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/XCLighting/XCLightManager.h"
#include "Graphics/GPUResourceSystem.h"

#include "Engine/GameplayBase/WorldEventTypes.h"
#include "Engine/Resource/ResourceManager.h"
#include "Engine/Event/EventBroadcaster.h"

Terrain::Terrain()
    : m_pHeightMapFileName("")
    , m_pBitmapImage(nullptr)
    , m_OBBHierarchy(nullptr)
{
    m_material.Ambient = XCVec4Unaligned(0.1f, 0.1f, 0.1f, 1.0f);
    m_material.Diffuse = XCVec4Unaligned(0.48f, 0.77f, 0.46f, 1.0f);
    m_material.Specular = XCVec4Unaligned(0.2f, 0.2f, 0.2f, 16.0f);

    m_useShaderType = ShaderType_TerrainMultiTexture;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TERRAIN;

    m_textures.clear();
}

Terrain::~Terrain(void)
{
    XCDELETEARRAY(m_pBitmapImage);
}

IActor::ActorReturnState Terrain::LoadMetaData( const void* metaData )
{
    const FBMultiTexturedTerrain* fbTerrainBuff = (FBMultiTexturedTerrain*)metaData;

    SimpleTerrain::LoadMetaData(fbTerrainBuff->Base());

    m_pHeightMapFileName = getPlatformPath(fbTerrainBuff->BitmapFileName()->c_str());

    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName1()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DResourceName2()->c_str()));
    m_textures.push_back(&resMgr.AcquireResource(fbTerrainBuff->Texture2DBlendResourceName3()->c_str()));

    ComputeVertices();

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_pCBPerObject = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(GPUResourceType_CBV, sizeof(PerObjectBuffer)));

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState Terrain::Load()
{
    LoadHeightMap();
    GenerateVertices();
    GenerateIndicesWithTextureMapping();
    GenerateVerticesNormal();
    BuildGeometryBuffer();
    UnloadHeightMap();

    XCVec3 vec = XCVec3(0, 0, 0);
    InitXPhysics(m_currentPosition, XCVec4(vec), XCVec4(vec), 1000, (f32)0.2); //Immovable

    return IActor::Load();
}

IActor::ActorReturnState Terrain::OnLoaded()
{
    for (auto& tex : m_textures)
    {
        if (!tex->m_Resource->IsLoaded())
        {
            return IActor::ActorReturnState_Processing;
        }
    }

    //Terrain is loaded, so fire up the physics
    Event_Scene event(EventType_TerrainLoaded);
    EventBroadcaster& broadcaster = (EventBroadcaster&)SystemLocator::GetInstance()->RequestSystem("EventBroadcaster");
    broadcaster.BroadcastEvent(&event);

    return IActor::OnLoaded();
}

void Terrain::GenerateVertices()
{
    //Set the position of every vertex
    m_vertexPosNormTexBuffer.m_vertexData.resize(m_totalVertices);

    i32 verticesIndex = 0;
    i32 bitmapRGBIndex = 0;

    //Generate the obb tree to optimize terrain - actor collisions.
    m_OBBHierarchy = XCNEW(TerrainOBBHierarchy)();
    m_OBBHierarchy->CreateTerrainOBBHierarchy(0, m_rows, 0, m_cols, m_cols);

    for (i32 rowIndex = 0; rowIndex < m_rows; rowIndex++)
    {
        for (i32 colIndex = 0; colIndex < m_cols; colIndex++)
        {
            f32 x = m_currentPosition.Get<X>() - (colIndex * m_rowSpacing);
            f32 z = m_currentPosition.Get<Z>() + (rowIndex * m_colSpacing);
      
            //float y = m_initialPosition.y + (float)GetHeightAt( (m_rows * rowIndex) + colIndex)/15.0f;
            //float y = m_initialPosition.y + ((float) GetHeightAt(k) / 10.0f);
            f32 y = (f32) GetHeightAt(bitmapRGBIndex) / 50.0f;

            //Necessary increment by 3, as the bitmap contains rgb format at each pixel, thus increment by 3.
            bitmapRGBIndex += 3;

            switch(m_useShaderType)
            {
                case ShaderType_LightTexture:
                case ShaderType_TerrainMultiTexture:
                    {
                        XCVec3 pos(x, y, z);
                        m_vertexPosNormTexBuffer.m_vertexData[verticesIndex++] = VertexPosNormTex(XCVec3Unaligned(x, y, z), XCVec3Unaligned(0.0f, 1.0f, 0.0f), XCVec2Unaligned(0.0f, 1.0f));
                        m_OBBHierarchy->ComputeQuad(rowIndex, colIndex, XCVec4(pos));
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

    for(u32 vertexIndex = 0; vertexIndex < m_indexBuffer.m_indexData.size() - 3; vertexIndex += 3)
    {
        XCVec4 v1(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex]].Pos);
        XCVec4 v2(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 1]].Pos);
        XCVec4 v3(m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 2]].Pos);

        XCVec4 vertexNormal = GetNormalFromPoints(v1, v2, v3);

        m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex]].Norm = vertexNormal.GetUnaligned3();

        vertexNormal = GetNormalFromPoints(v2, v1, v3);
        m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 1]].Norm = vertexNormal.GetUnaligned3();

        vertexNormal = GetNormalFromPoints(v3, v1, v2);
        m_vertexPosNormTexBuffer.m_vertexData[m_indexBuffer.m_indexData[vertexIndex + 2]].Norm, vertexNormal.GetUnaligned3();
    }
}

XCVec4 Terrain::CheckTerrainCollisionFromPoint(OrientedBoundingBox* bbox)
{
    TerrainQuad* quad = m_OBBHierarchy->GetQuadCollidingWithOBB(bbox);
    XCVec4 vertexPos;

    if (quad != nullptr)
    {
        //Find the vertex within the terrain quad
        for (i32 rowIndex = quad->m_rowStart; rowIndex < quad->m_rowEnd; rowIndex++)
        {
            for (i32 colIndex = quad->m_colStart; colIndex < quad->m_colEnd; colIndex++)
            {
                vertexPos = XCVec4(m_vertexPosNormTexBuffer.m_vertexData[quad->m_totalWidth * rowIndex + colIndex].Pos);

                DirectX::ContainmentType contain = bbox->m_TransformedBox.Contains(vertexPos.GetPlatformIntrinsic());
                if (contain == DirectX::CONTAINS || contain == DirectX::INTERSECTS)
                {
                    return vertexPos;
                }
            }
        }
    }

    //If we come here that means no collision with any terrain vertex. So return nothing
    return XCVec4();
}


void Terrain::LoadHeightMap()
{
    FILE *pFilePtr;
    i32 error;

    u32 count;

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

    i32 imageSize = m_rows * m_cols * 3;

    m_pBitmapImage = XCNEW(u8)[imageSize];

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
    XCDELETEARRAY(m_pBitmapImage);
}

char Terrain::GetHeightAt(i32 _index) const
{
    return m_pBitmapImage[_index];
}

XCVec4 Terrain::GetPointAtIndex(i32 pointIndex) const
{
    XCVec3Unaligned vec = m_vertexPosNormTexBuffer.m_vertexData[pointIndex].Pos;
    return XCVec4(vec);
}

IActor::ActorReturnState Terrain::Update(f32 dt)
{
    m_OBBHierarchy->Transform(XCMatrix(), XCMatrix());
    m_OBBHierarchy->Update(dt);

    return SimpleTerrain::Update(dt);
}

bool Terrain::Draw(RenderContext& renderContext)
{
    renderContext.ApplyShader(m_useShaderType);

    XCShaderHandle* shaderHandle = nullptr;

    // Set constants
    ICamera& cam = renderContext.GetGlobalShaderData().m_camera;
    PerObjectBuffer perObject = {
        MatrixTranspose(m_World).GetUnaligned(),
        MatrixTranspose(m_World * cam.GetViewMatrix() * cam.GetProjectionMatrix()).GetUnaligned(),
        MatrixInverseTranspose(m_World).GetUnaligned(),
        XCMatrix().GetUnaligned(),
        m_material
    };
    m_pCBPerObject->UploadDataOnGPU(renderContext.GetDeviceContext(), &perObject, sizeof(PerObjectBuffer));

    if (m_useShaderType == ShaderType_LightTexture)
    {
        shaderHandle = (XCShaderHandle*)renderContext.GetShader(ShaderType_LightTexture);
        shaderHandle->SetConstantBuffer("PerObjectBuffer", renderContext.GetDeviceContext(), *m_pCBPerObject);
        shaderHandle->SetResource("gDiffuseMap", renderContext.GetDeviceContext(), m_textures[0]);
    }
    else if (m_useShaderType == ShaderType_TerrainMultiTexture)
    {
        shaderHandle = (XCShaderHandle*)renderContext.GetShader(ShaderType_TerrainMultiTexture);
        shaderHandle->SetConstantBuffer("PerObjectBuffer", renderContext.GetDeviceContext(), *m_pCBPerObject);
        shaderHandle->SetResource("gDiffuseMap",  renderContext.GetDeviceContext(), m_textures[0]);
        shaderHandle->SetResource("gDiffuseMap1", renderContext.GetDeviceContext(), m_textures[1]);
        shaderHandle->SetResource("gDiffuseMap2", renderContext.GetDeviceContext(), m_textures[2]);
        shaderHandle->SetResource("gBlendMap",    renderContext.GetDeviceContext(), m_textures[3]);
    }

#if defined(FORWARD_LIGHTING)
    XCLightManager* lightMgr = (XCLightManager*)&SystemLocator::GetInstance()->RequestSystem("LightsManager");
    shaderHandle->SetConstantBuffer("cbLightsPerFrame", renderContext.GetDeviceContext(), lightMgr->GetLightConstantBuffer());
#endif

    shaderHandle->SetVertexBuffer(renderContext.GetDeviceContext(), &m_vertexPosNormTexBuffer);
    shaderHandle->SetIndexBuffer(renderContext.GetDeviceContext(), m_indexBuffer);

    renderContext.DrawIndexedInstanced(m_indexBuffer.m_indexData.size(), m_indexBuffer.GetIndexBufferInGPUMem());

    m_OBBHierarchy->Draw(renderContext);

    return true;
}

IActor::ActorReturnState Terrain::Destroy()
{
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");

    for (auto& tex : m_textures)
    {
        resMgr.ReleaseResource(tex);
    }
    
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    gpuSys.DestroyResource(m_pCBPerObject);

    XCDELETE(m_OBBHierarchy);

    return SimpleTerrain::Destroy();
}

XCVec3 Terrain::GetTerrainNormal(f32 x, f32 z) const
{
    return XCVec3(0,0,0);
}