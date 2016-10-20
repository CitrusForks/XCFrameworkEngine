/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "Waves.h"
#include "Graphics/XCGraphicsDx11.h"

Waves::Waves(void)
{
}

Waves::~Waves(void)
{
}

void Waves::PreLoad(const void* fbBuffer)
{
    const FBWaves* fbWavesBuff = (FBWaves*)fbBuffer;

    SimpleTerrain::PreLoad(fbWavesBuff->Base());

    ComputeVertices();

    m_useShaderType = ShaderType_SolidColor;
    m_collisionDetectionType = COLLISIONDETECTIONTYPE_TRIANGLE;
}

void Waves::Update(f32 dt)
{
    SimpleTerrain::Update(dt);
 
    XCGraphics& graphicsSystem = (XCGraphics&)SystemLocator::GetInstance()->RequestSystem("GraphicsSystem");

#if defined(TODO)
    //Create moving terrain using Dynamic Vertex Buffer
    D3D11_MAPPED_SUBRESOURCE mappedData;
    ValidateResult(graphicsSystem.GetDeviceContext()->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    VertexPosColor* v = (VertexPosColor*)mappedData.pData;
    for (i32 vertexIndex = 0; vertexIndex < m_totalVertices; vertexIndex++)
    {
        m_vertices[vertexIndex].Pos.y = (f32) (rand() % 5);
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