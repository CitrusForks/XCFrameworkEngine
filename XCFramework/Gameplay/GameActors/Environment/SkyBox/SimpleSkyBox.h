/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Actors/SimpleActor.h"

#include "Engine/Resource/ResourceHandle.h"

#include "Graphics/XCShaders/XCShaderTypes.h"
#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCMaterials/MaterialTypes.h"
#include "Graphics/XCShaders/XCRasterizerTypes.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

class SimpleSkyBox : public SimpleActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleSkyBox)

    SimpleSkyBox(void);
    virtual ~SimpleSkyBox(void);

    IActor::ActorReturnState Init() override;
    IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    IActor::ActorReturnState Load() override;
    IActor::ActorReturnState Update(f32 dt) override;
    bool                     Draw(RenderContext& renderContext) override;
    IActor::ActorReturnState Destroy() override;

    void                            BuildBuffers();

protected:
    u32                             m_Stride;
    u32                             m_Offset;

    ResourceHandle*                 m_cubeMapTexture;

    ShaderType                      m_useShaderType;
    
    VertexBuffer<VertexPos>         m_vertexBuffer;
    IndexBuffer<u32>                m_indexBuffer;

    GPUResource*                    m_CBwvp;
};