/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/EngineUtils.h"
#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/PhysicsActor.h"
#include "Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

class SimpleTerrain : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleTerrain)

    SimpleTerrain(void);
    virtual ~SimpleTerrain(void);
 
    virtual void                        PreLoad(XCVec3& _initialPosition, i32 _rows, i32 _column, f32 _rowSpacing, f32 _colSpacing);
    virtual void                        Load();
    virtual void                        GenerateVertices();
    virtual void                        GenerateIndicesWithTextureMapping();
    virtual void                        BuildGeometryBuffer();
    virtual void                        Update(f32 dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                ComputeVertices();
    f32                               GetHeight(f32 _x, f32 _z); //Builds a hill height
    virtual i32                         GetNoOfVertices() const         { return m_totalVertices;  }

protected:
    XCVec3                              m_initialPosition;
    i32                                 m_rows;
    i32                                 m_cols;
    i32                                 m_totalVertices;
    i32                                 m_totalIndices;
    f32                               m_rowSpacing;
    f32                               m_colSpacing;

    VertexBuffer<VertexPosColor>        m_vertexPosColorBuffer;
    VertexBuffer<VertexPosNormTex>      m_vertexPosNormTexBuffer;
    IndexBuffer<u32>           m_indexBuffer;

    ShaderType                          m_useShaderType;

private:
    D3DConstantBuffer*                  m_pCBPerObject;
};