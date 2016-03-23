/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Plane.h"
#include "Gameplay/GameActors/SimpleActor.h"
#include "Engine/Graphics/XC_Textures/Texture2D.h"
#include "Engine/Graphics/XC_Textures/RenderableTexture.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Engine/Graphics/VertexBuffer.h"

class TexturedPlane : public Plane, public SimpleActor
{
public:
    DECLARE_OBJECT_CREATION(TexturedPlane)

    TexturedPlane();
    TexturedPlane(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3);
    virtual ~TexturedPlane();

    virtual void                     Init(int actorId);
    virtual void                     PreLoad(const void* fbBuffer);
    virtual void                     PreLoad(XCVecIntrinsic4 initialPosition, XCVecIntrinsic4 initialRotation, XCVecIntrinsic4 initialScaling, BasicMaterial material, Texture2D* texture, RasterType rasterType);
    virtual void                     Load();
    virtual void                     Update(float dt);
    virtual void                     Draw(RenderContext& renderContext);
    virtual void                     Destroy();
    virtual void                     BuildGeometryBuffers();

protected:
    ShaderType                        m_useShaderType;
    BasicMaterial                     m_material;
    Texture2D*                        m_texture;
    VertexBuffer<VertexPosNormTex>    m_vertexBuffer;
    RasterType                        m_rasterType;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*                m_pCBPerObject;
#endif
};