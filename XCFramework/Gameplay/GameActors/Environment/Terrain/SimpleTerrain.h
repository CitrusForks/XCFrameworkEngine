/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Actors/PhysicsActor.h"
#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCShaders/XCShaderTypes.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

class SimpleTerrain : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleTerrain)

    SimpleTerrain(void);
    virtual ~SimpleTerrain(void);
 
    virtual void                        PreLoad(const void* fbBuffer) override;
    virtual void                        Load() override;
    virtual void                        Update(f32 dt) override;
    virtual void                        Draw(RenderContext& context) override;
    virtual void                        Destroy() override;

    i32                                 GetNoOfVertices() const { return m_totalVertices; }

protected:
    virtual void                        BuildGeometryBuffer();
    virtual void                        GenerateVertices();

    void                                ComputeVertices();
    void                                GenerateIndicesWithTextureMapping();
    f32                                 GetHeight(f32 _x, f32 _z); //Builds a hill height

    //Member Variables
    i32                                 m_rows;
    i32                                 m_cols;
    i32                                 m_totalVertices;
    i32                                 m_totalIndices;
    f32                                 m_rowSpacing;
    f32                                 m_colSpacing;

    VertexBuffer<VertexPosColor>        m_vertexPosColorBuffer;
    VertexBuffer<VertexPosNormTex>      m_vertexPosNormTexBuffer;
    IndexBuffer<u32>                    m_indexBuffer;

    ShaderType                          m_useShaderType;
    GPUResource*                        m_pCBPerObject;
};