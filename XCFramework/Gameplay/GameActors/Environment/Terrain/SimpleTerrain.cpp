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

#include "Gameplay/XCCamera/XCCameraManager.h"

SimpleTerrain::SimpleTerrain(void)
{
    m_useShaderType = ShaderType_SolidColor;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TRIANGLE;
}

SimpleTerrain::~SimpleTerrain(void)
{
}

void SimpleTerrain::PreLoad(XCVec3& _initialPosition, i32 _rows, i32 _column, f32 _rowSpacing, f32 _colSpacing)
{
    //Get initial position
    m_initialPosition = _initialPosition;
    m_rows = _rows;
    m_cols = _column;
    m_rowSpacing = _rowSpacing;
    m_colSpacing = _colSpacing;

    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    m_pCBPerObject = gpuSys.CreateConstantBufferResourceView(GPUResourceDesc(GPUResourceType_CBV, sizeof(PerObjectBuffer)));
}

void SimpleTerrain::Load()
{
    GenerateVertices();
    GenerateIndicesWithTextureMapping();

    BuildGeometryBuffer();
    PhysicsActor::Load();
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
            f32 x = m_initialPosition.Get<X>() - (colIndex * m_rowSpacing);
            f32 z = m_initialPosition.Get<Z>() + (rowIndex * m_colSpacing);
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

void SimpleTerrain::Update(f32 dt)
{
}

void SimpleTerrain::Draw(RenderContext& context)
{
    context.ApplyShader(m_useShaderType);
    
    // Set constants
    cbWorld wbuffer = { MatrixTranspose(m_World).GetUnaligned() };
    ICamera& cam = context.GetGlobalShaderData().m_camera;
    
    cbWVP perObject = {
        MatrixTranspose(cam.GetViewMatrix() * cam.GetProjectionMatrix() * m_World).GetUnaligned(),
    };
    m_pCBPerObject->UploadDataOnGPU(context.GetDeviceContext(), &perObject, sizeof(PerObjectBuffer));
    
    XCShaderHandle* solidColorShader = (XCShaderHandle*)context.GetShader(m_useShaderType);
    solidColorShader->SetVertexBuffer(context.GetDeviceContext(), &m_vertexPosColorBuffer);
    solidColorShader->SetIndexBuffer(context.GetDeviceContext(), m_indexBuffer);

    solidColorShader->SetConstantBuffer("cbWVP", context.GetDeviceContext(), *m_pCBPerObject);


    context.DrawIndexedInstanced(m_indexBuffer.m_indexData.size());
}

void SimpleTerrain::Destroy()
{
    GPUResourceSystem& gpuSys = (GPUResourceSystem&)SystemLocator::GetInstance()->RequestSystem("GPUResourceSystem");
    gpuSys.DestroyResource(m_pCBPerObject);
}