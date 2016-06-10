/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "SimpleTerrain.h"
#include "Graphics/XC_GraphicsDx11.h"
#include "Graphics/XC_Shaders/XC_ShaderBufferConstants.h"
#include "Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Gameplay/XC_Camera/XC_CameraManager.h"
#include "Graphics/XC_Shaders/XC_ShaderHandle.h"

SimpleTerrain::SimpleTerrain(void)
{
    m_useShaderType = ShaderType_SolidColor;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TRIANGLE;
    m_useRenderWorkerType = WorkerType_Terrain;
}

SimpleTerrain::~SimpleTerrain(void)
{
}

void SimpleTerrain::PreLoad(XCVec3 _initialPosition, int _rows, int _column, float _rowSpacing, float _colSpacing)
{
    //Get initial position
    m_initialPosition = _initialPosition;
    m_rows = _rows;
    m_cols = _column;
    m_rowSpacing = _rowSpacing;
    m_colSpacing = _colSpacing;

    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    m_pCBPerObject = heap.CreateBufferView(D3DBufferDesc(BUFFERTYPE_CBV, sizeof(PerObjectBuffer)));
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

    int verticesIndex = 0;

    for (int rowIndex = 0; rowIndex < m_rows; rowIndex++)
    {
        for (int colIndex = 0; colIndex < m_cols; colIndex++)
        {
#if defined(WIN32)
            float x = m_initialPosition.x - (colIndex * m_rowSpacing);
            float z = m_initialPosition.z + (rowIndex * m_colSpacing);
#elif defined(XC_ORBIS)
            float x = m_initialPosition.getX() - (colIndex * m_rowSpacing);
            float z = m_initialPosition.getZ() + (rowIndex * m_colSpacing);
#endif
            float y = GetHeight(x, z);

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
            m_vertexPosColorBuffer.m_vertexData[verticesIndex++] = VertexPosColor(XCVec3Unaligned(x, y, z), ToXCVec4Unaligned(color));
            //(const float*)&Colors::Red);
        }
    }
}

void SimpleTerrain::GenerateIndicesWithTextureMapping()
{
    //Set the indices
    unsigned int faceCount = (m_rows - 1) * (m_cols - 1) * 2;

    m_indexBuffer.m_indexData.resize(faceCount * 3);
    m_totalIndices = m_indexBuffer.m_indexData.size();
    int indicesIndex = 0;

    float u = 1.0f / (m_rows - 1);
    float v = 1.0f / (m_cols - 1);

    for(int rowIndex = 0; rowIndex < m_rows - 1; rowIndex++)
    {
        for(int colIndex = 0; colIndex < m_cols - 1 ; colIndex++)
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
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

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

float SimpleTerrain::GetHeight(float _x, float _z)
{
    return 0.3f*(_z*sinf(0.1f*_x) + _x*cosf(0.1f*_z));
}

void SimpleTerrain::Update(float dt)
{
}

void SimpleTerrain::Draw(RenderContext& context)
{
    context.SetRasterizerState(RasterType_FillSolid);
    context.ApplyShader(m_useShaderType);
    
    // Set constants
    cbWorld wbuffer = { ToXCMatrix4Unaligned(XMMatrixTranspose(m_World)) };
    ICamera& cam = context.GetShaderManagerSystem().GetGlobalShaderData().m_camera;
    cbWVP perObject = {
        ToXCMatrix4Unaligned(XMMatrixTranspose(cam.GetViewMatrix() * cam.GetProjectionMatrix() * m_World)),
    };

    XCShaderHandle* solidColorShader = (XCShaderHandle*)context.GetShaderManagerSystem().GetShader(m_useShaderType);
    memcpy(m_pCBPerObject->m_cbDataBegin, &perObject, sizeof(PerObjectBuffer));
    solidColorShader->SetConstantBuffer("cbWVP", context.GetDeviceContext(), *m_pCBPerObject);
    solidColorShader->SetVertexBuffer(context.GetDeviceContext(), &m_vertexPosColorBuffer);
    solidColorShader->SetIndexBuffer(context.GetDeviceContext(), m_indexBuffer);

    context.GetShaderManagerSystem().DrawIndexedInstanced(context.GetDeviceContext(), m_indexBuffer.m_indexData.size());
}

void SimpleTerrain::Destroy()
{
    SharedDescriptorHeap& heap = (SharedDescriptorHeap&)SystemLocator::GetInstance()->RequestSystem("SharedDescriptorHeap");
    heap.DestroyBuffer(m_pCBPerObject);
}