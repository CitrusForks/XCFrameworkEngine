/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if UNUSED

#include "Engine/Graphics/BasicGeometry/CubeMesh.h"
#include "Engine/Graphics/XC_Graphics.h"
#include "Engine/Graphics/XCShaders/XCVertexShaderLayout.h"

#include "Engine/Resource/ResourceManager.h"

CubeMesh::CubeMesh(void)
{
    m_isBufferBuild = false;
}

CubeMesh::~CubeMesh(void)
{
}

void CubeMesh::Init()
{
    m_material.Ambient  = XCVec4(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.Diffuse  = XCVec4(0.5f, 0.8f, 0.0f, 1.0f);
    m_material.Specular = XCVec4(0.2f, 0.2f, 0.2f, 16.0f);

    ResourceManager& resMgr = (ResourceManager&)SystemLocator::GetInstance()->RequestSystem("ResourceManager");
    m_texture = (Texture2D*)resMgr.AcquireResource("Sand");

    m_useShaderType = ShaderType_LightTexture;

    m_rasterType = RasterType_FillSolid;

    Logger("[CubeMesh] Init done");
}

void CubeMesh::PreLoad(XCVec4 initialPosition, XCVec4 initialRotation, XCVec4 initialScaling, Material material, Texture2D* texture, RasterType rasterType)
{
    m_currentPosition = initialPosition;
    m_MTranslation = XMMatrixTranslation(XMVectorGetX(initialPosition), XMVectorGetY(initialPosition), XMVectorGetZ(initialPosition));
    ApplyRotation(initialRotation);
    m_material = material;
    if (m_texture)
        m_texture = texture;
    m_rasterType = rasterType;
    Logger("[CubeMesh] Preload done");
}

void CubeMesh::Load()
{
    BuildBuffers();
    Logger("[CubeMesh] Load done");
}

void CubeMesh::BuildBuffers()
{
#if defined(DEBUG_OBB)
    if (!m_isBufferBuild)
    {
        XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

        //Set up vertices
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(-1.0f, -1.0f, -1.0f), XCVec3(-0.33f, -0.33f, -0.33f), XCVec2(0.0f, 1.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(-1.0f, 1.0f, -1.0f), XCVec3(-0.33f, 0.33f, -0.33f), XCVec2(0.0f, 0.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(1.0f, 1.0f, -1.0f), XCVec3(0.33f, 0.33f, -0.33f), XCVec2(1.0f, 0.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(1.0f, -1.0f, -1.0f), XCVec3(0.33f, -0.33f, -0.33f), XCVec2(1.0f, 1.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(-1.0f, -1.0f, 1.0f), XCVec3(-0.33f, -0.33f, 0.33f), XCVec2(0.0f, 1.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(-1.0f, 1.0f, 1.0f), XCVec3(-0.33f, 0.33f, 0.33f), XCVec2(0.0f, 0.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(1.0f, 1.0f, 1.0f), XCVec3(0.33f, 0.33f, 0.33f), XCVec2(1.0f, 0.0f)));
        m_vertexBuffer.m_vertexData.push_back(VertexPosNormTex(XCVec3(1.0f, -1.0f, 1.0f), XCVec3(0.33f, -0.33f, 0.33f), XCVec2(1.0f, 1.0f)));
         
        /*
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(-1.0f, -1.0f, -1.0f), Colors::White));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(-1.0f, 1.0f, -1.0f), Colors::Black));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(1.0f, 1.0f, -1.0f), Colors::Red));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(1.0f, -1.0f, -1.0f), Colors::Green));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(-1.0f, -1.0f, 1.0f), Colors::Blue));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(-1.0f, 1.0f, 1.0f), Colors::Yellow));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(1.0f, 1.0f, 1.0f), Colors::Cyan));
        m_vertexBuffer.m_vertexData.push_back(VertexPosColor(XMFLOAT3(1.0f, -1.0f, 1.0f), Colors::Magenta));
        */
        m_vertexBuffer.BuildVertexBuffer();

        m_Stride = sizeof(VertexPosNormTex);
        m_Offset = 0;

        //Set up index buffer
        m_indexBuffer.AddIndicesVA
        ({   0, 1, 2,
            0, 2, 3,

            4, 6, 5,
            4, 7, 6,

            4, 5, 1,
            4, 1, 0,

            3, 2, 6,
            3, 6, 7,

            1, 5, 6,
            1, 6, 2,

            4, 0, 3,
            4, 3, 7 });

        m_indexBuffer.BuildIndexBuffer();

#if !defined(XCGRAPHICS_DX12)
        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(indices);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        //ibd.StructureByteStride = 0;
        ibd.StructureByteStride = sizeof(WORD);

        D3D11_SUBRESOURCE_DATA iInitData;
        iInitData.pSysMem = indices;
        iInitData.SysMemPitch = 0;
        iInitData.SysMemSlicePitch = 0;

        ValidateResult(graphicsSystem.GetDevice()->CreateBuffer(&ibd, &iInitData, &m_pIB));
        XCASSERT(m_pIB);
#endif
        m_isBufferBuild = true;
    }
#endif
}

void CubeMesh::Update(f32 dt)
{
    m_World = m_MScaling * m_MRotation * m_MTranslation;
}

void CubeMesh::Draw(RenderContext& context)
{
#if defined(DEBUG_OBB)
    context.SetRasterizerState(m_rasterType);
    context.ApplyShader(m_useShaderType);

    context.GetDeviceContext().IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context.GetDeviceContext().IASetVertexBuffers(0, 1, &m_pVB, &m_Stride, &m_Offset);
    context.GetDeviceContext().IASetIndexBuffer(m_pIB, DXGI_FORMAT_R16_UINT, m_Offset);

    // Set constants
    cbWorld wbuffer = { XMMatrixTranspose(m_World) };
    cbInvTransposeWorld invTransWorld = { MatrixInverseTranspose(m_World) };
    cbMatTexPerObject matTexPerObject = { *m_texture->GetTextureCoordinateMatrix(), m_material };

    if (m_useShaderType == ShaderType_LightTexture)
    {
        LightTextureShader* lightTexShader = (LightTextureShader*)context.GetShader(ShaderType_LightTexture);
        lightTexShader->setCBWorld(context.GetDeviceContext(), &wbuffer);
        lightTexShader->setCBInvTransposeMatrix(context.GetDeviceContext(), &invTransWorld);
        lightTexShader->setCBMatTexPerObject(context.GetDeviceContext(), &matTexPerObject);
        lightTexShader->setTexture2D(context.GetDeviceContext(), m_texture->GetTextureResource());
    }
    else
    {
        //Solid Shader
        SolidColorShader* solidColorShader = (SolidColorShader*)context.GetShader(ShaderType_SolidColor);
        solidColorShader->setCBWorld(context.GetDeviceContext(), wbuffer);
    }

    context.DrawIndexedInstanced(36);
#endif
}

void CubeMesh::Destroy()
{
    m_texture = nullptr;
}
#endif