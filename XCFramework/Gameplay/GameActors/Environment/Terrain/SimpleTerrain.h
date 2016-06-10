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
 
    virtual void                        PreLoad(XCVec3 _initialPosition, int _rows, int _column, float _rowSpacing, float _colSpacing);
    virtual void                        Load();
    virtual void                        GenerateVertices();
    virtual void                        GenerateIndicesWithTextureMapping();
    virtual void                        BuildGeometryBuffer();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                ComputeVertices();
    float                               GetHeight(float _x, float _z); //Builds a hill height
    virtual int                         GetNoOfVertices() const         { return m_totalVertices;  }

protected:
    XCVec3                              m_initialPosition;
    int                                 m_rows;
    int                                 m_cols;
    int                                 m_totalVertices;
    int                                 m_totalIndices;
    float                               m_rowSpacing;
    float                               m_colSpacing;

    VertexBuffer<VertexPosColor>        m_vertexPosColorBuffer;
    VertexBuffer<VertexPosNormTex>      m_vertexPosNormTexBuffer;
    IndexBuffer<unsigned int>           m_indexBuffer;

    ShaderType                          m_useShaderType;

private:
    D3DConstantBuffer*                  m_pCBPerObject;
};