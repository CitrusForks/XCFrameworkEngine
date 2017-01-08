/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "SimpleTerrain.h"

#include "Engine/Utils/EngineUtils.h"

#include "Graphics/XCGraphics.h"
#include "Graphics/XCShaders/XCShaderBufferConstants.h"
#include "Graphics/XCShaders/XCShaderHandle.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/GPUResourceSystem.h"

SimpleTerrain::SimpleTerrain(void)
    : m_rows(0)
    , m_cols(0)
    , m_totalVertices(0)
    , m_totalIndices(0)
    , m_rowSpacing(0)
    , m_colSpacing(0)
    , m_useShaderType(ShaderType_SolidColor)
    , m_pCBPerObject(nullptr)
{
}

SimpleTerrain::~SimpleTerrain(void)
{
}

IActor::ActorReturnState SimpleTerrain::LoadMetaData( const void* metaData )
{
    const FBSimpleTerrain* fbSimpleTerrainBuff = (FBSimpleTerrain*)metaData;

    PhysicsActor::LoadMetaData(fbSimpleTerrainBuff->Base());

    m_rows = fbSimpleTerrainBuff->Rows();
    m_cols = fbSimpleTerrainBuff->Column();
    m_rowSpacing = (f32) fbSimpleTerrainBuff->RowSpacing();
    m_colSpacing = (f32) fbSimpleTerrainBuff->ColSpacing();

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_pCBPerObject = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(GPUResourceType_CBV, sizeof(PerObjectBuffer)));

    return IActor::ActorReturnState_Success;
}

IActor::ActorReturnState SimpleTerrain::Load()
{
    GenerateVertices();
    GenerateIndicesWithTextureMapping();

    BuildGeometryBuffer();
    return PhysicsActor::Load();
}

void SimpleTerrain::ComputeVertices()
{
    m_totalVertices = m_rows * m_cols;
    m_totalIndices = 0;
}

void SimpleTerrain::GenerateVertices()
{
    //Set the position of every vertex
    m_vertexPosColorBuffer.m_vertexData.resize(m_totalVertices);

    i32 verticesIndex = 0;

    for (i32 rowIndex = 0; rowIndex < m_rows; rowIndex++)
    {
        for (i32 colIndex = 0; colIndex < m_cols; colIndex++)
        {
            f32 x = m_currentPosition.Get<X>() - (colIndex * m_rowSpacing);
            f32 z = m_currentPosition.Get<Z>() + (rowIndex * m_colSpacing);
            f32 y = GetHeight(x, z);

            XCVec4 color;
            switch (rand() % 3)
            {
            case 1: color = Colors::Red; break;
            case 2: color = Colors::Blue; break;
            case 0: color = Colors::Magenta; break;
            default: color =Colors::Black; break;
            }
            // Color the vertex based on its height.
            if (y < -10.0f)
            {
                // Sandy beach color.
                color = XCVec4(1.0f, 0.96f, 0.62f, 1.0f);
            }
            else if (y < 5.0f)
            {
                // Light yellow-green.
                color = XCVec4(0.48f, 0.77f, 0.46f, 1.0f);
            }
            else if (y < 12.0f)
            {
                // Dark yellow-green.
                color = XCVec4(0.1f, 0.48f, 0.19f, 1.0f);
            }
            else if (y < 20.0f)
            {
                // Dark brown.
                color = XCVec4(0.45f, 0.39f, 0.34f, 1.0f);
            }
            else
            {
                // White snow.
                color = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            m_vertexPosColorBuffer.m_vertexData[verticesIndex++] = VertexPosColor(XCVec3Unaligned(x, y, z), color.GetUnaligned4());
            //(const float*)&Colors::Red);
        }
    }
}

void SimpleTerrain::GenerateIndicesWithTextureMapping()
{
    //Set the indices
    u32 faceCount = (m_rows - 1) * (m_cols - 1) * 2;

    m_indexBuffer.m_indexData.resize(faceCount * 3);
    m_totalIndices = m_indexBuffer.m_indexData.size();
    i32 indicesIndex = 0;

    f32 u = 1.0f / (m_rows - 1);
    f32 v = 1.0f / (m_cols - 1);

    for(i32 rowIndex = 0; rowIndex < m_rows - 1; rowIndex++)
    {
        for(i32 colIndex = 0; colIndex < m_cols - 1 ; colIndex++)
        {
            m_indexBuffer.m_indexData[indicesIndex] = rowIndex * m_cols + colIndex;
            m_indexBuffer.m_indexData[indicesIndex + 1] = rowIndex * m_cols + (colIndex + 1);
            m_indexBuffer.m_indexData[indicesIndex + 2] = (rowIndex + 1) * m_cols + (colIndex + 1);
            
            m_indexBuffer.m_indexData[indicesIndex + 3] = rowIndex * m_cols + colIndex;
            m_indexBuffer.m_indexData[indicesIndex + 4] = (rowIndex + 1) * m_cols + (colIndex + 1);
            m_indexBuffer.m_indexData[indicesIndex + 5] = (rowIndex + 1) * m_cols + colIndex;

            if (m_useShaderType == ShaderType_LightTexture || m_useShaderType == ShaderType_TerrainMultiTexture)
            {
                //Set Texture mapping
                //Vertex 1
                m_vertexPosNormTexBuffer.m_vertexData[rowIndex * m_cols + colIndex].Tex             = XCVec2Unaligned(rowIndex * u, colIndex * v);
                m_vertexPosNormTexBuffer.m_vertexData[rowIndex * m_cols + (colIndex + 1)].Tex       = XCVec2Unaligned(rowIndex * u, (colIndex + 1)* v);
                m_vertexPosNormTexBuffer.m_vertexData[(rowIndex + 1) * m_cols + (colIndex + 1)].Tex = XCVec2Unaligned((rowIndex + 1) * u, (colIndex + 1) * v);
                m_vertexPosNormTexBuffer.m_vertexData[(rowIndex + 1) * m_cols + colIndex].Tex       = XCVec2Unaligned((rowIndex + 1) * u, colIndex * v);
            }
            indicesIndex += 6;
        }
    }
}

void SimpleTerrain::BuildGeometryBuffer()
{
    XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

    //Allocate resource for vb and ib
    if (m_useShaderType == ShaderType_SolidColor)
    {
        m_vertexPosColorBuffer.BuildVertexBuffer();
        m_indexBuffer.BuildIndexBuffer();
    }
    else if (m_useShaderType == ShaderType_LightTexture || m_useShaderType == ShaderType_TerrainMultiTexture)
    {
        m_vertexPosNormTexBuffer.BuildVertexBuffer();
        m_indexBuffer.BuildIndexBuffer();
    }
}

f32 SimpleTerrain::GetHeight(f32 _x, f32 _z)
{
    return 0.3f*(_z*sinf(0.1f*_x) + _x*cosf(0.1f*_z));
}

IActor::ActorReturnState SimpleTerrain::Update(f32 dt)
{
    return PhysicsActor::Update(dt);
}

bool SimpleTerrain::Draw(RenderContext& renderContext)
{
    renderContext.ApplyShader(m_useShaderType);
    
    // Set constants
    cbWorld wbuffer = { MatrixTranspose(m_World).GetUnaligned() };
    ICamera& cam = renderContext.GetGlobalShaderData().m_camera;
    
    cbWVP perObject = {
        MatrixTranspose(cam.GetViewMatrix() * cam.GetProjectionMatrix() * m_World).GetUnaligned(),
    };
    m_pCBPerObject->UploadDataOnGPU(renderContext.GetDeviceContext(), &perObject, sizeof(PerObjectBuffer));
    
    XCShaderHandle* solidColorShader = (XCShaderHandle*)renderContext.GetShader(m_useShaderType);
    solidColorShader->SetVertexBuffer(renderContext.GetDeviceContext(), &m_vertexPosColorBuffer);
    solidColorShader->SetIndexBuffer(renderContext.GetDeviceContext(), m_indexBuffer);

    solidColorShader->SetConstantBuffer("cbWVP", renderContext.GetDeviceContext(), *m_pCBPerObject);


    renderContext.DrawIndexedInstanced(m_indexBuffer.m_indexData.size());

    return true;
}

IActor::ActorReturnState SimpleTerrain::Destroy()
{
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    gpuSys.DestroyResource(m_pCBPerObject);

    return PhysicsActor::Destroy();
}