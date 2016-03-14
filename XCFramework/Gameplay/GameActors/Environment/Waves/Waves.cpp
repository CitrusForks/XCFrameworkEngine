/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Waves.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"

Waves::Waves(void)
{
}

Waves::~Waves(void)
{
}

void Waves::Preload(const void* fbBuffer)
{
    const FBWaves* fbWavesBuff = (FBWaves*)fbBuffer;
    m_currentPosition = toXMVECTOR(fbWavesBuff->Position()->x(), fbWavesBuff->Position()->y(), fbWavesBuff->Position()->z(), 0);
    m_rows = fbWavesBuff->Rows();
    m_cols = fbWavesBuff->Column();
    m_rowSpacing = (float)fbWavesBuff->RowSpacing();
    m_colSpacing = (float)fbWavesBuff->ColSpacing();

    ComputeVertices();
}

void Waves::PreLoad(XCVec3 _initialPosition, int _rows, int _column, float _rowSpacing, float _colSpacing)
{
    SimpleTerrain::PreLoad(_initialPosition, _rows, _column, _rowSpacing, _colSpacing);

    m_useShaderType = SHADERTYPE_COLORTECH;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TRIANGLE;

    ComputeVertices();
}

void Waves::Update(float dt)
{
    SimpleTerrain::Update(dt);
 
    XC_Graphics& graphicsSystem = (XC_Graphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

#if defined(TODO)
    //Create moving terrain using Dynamic Vertex Buffer
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(graphicsSystem.GetDeviceContext()->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    VertexPosColor* v = (VertexPosColor*)mappedData.pData;
    for (int vertexIndex = 0; vertexIndex < m_totalVertices; vertexIndex++)
    {
        m_vertices[vertexIndex].Pos.y = (float) (rand() % 5);
        v[vertexIndex] = m_vertices[vertexIndex];
    }

    graphicsSystem.GetDeviceContext()->Unmap(m_pVB, 0);
#endif
}

void Waves::Draw(RenderContext& context)
{
    SimpleTerrain::Draw(context);
}

void Waves::Destroy()
{
    SimpleTerrain::Destroy();
}