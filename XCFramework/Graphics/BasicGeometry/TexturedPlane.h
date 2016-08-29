/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Plane.h"

#include "Engine/ObjectBase/IObjectDimensions.h"

#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Graphics/XC_Materials/MaterialTypes.h"
#include "Graphics/VertexBuffer.h"

class ResourceHandle;

class TexturedPlane : public IObjectDimensions, public IBase, public Plane
{
public:
    DECLARE_OBJECT_CREATION(TexturedPlane)

    TexturedPlane();
    TexturedPlane(XCVec4& p1, XCVec4& p2, XCVec4& p3);
    virtual ~TexturedPlane();

    virtual void                     PreLoad(const void* fbBuffer);
    virtual void                     PreLoad(XCVec4& initialPosition, XCVec4& initialRotation, XCVec4& initialScaling, BasicMaterial& material, std::string texture, RasterType rasterType);
    virtual void                     Load();
    virtual void                     Update(f32 dt);
    virtual void                     Draw(RenderContext& renderContext);
    virtual void                     Destroy();
    virtual void                     BuildGeometryBuffers();

protected:

    ShaderType                        m_useShaderType;
    ResourceHandle*                   m_texture;
    D3DConstantBuffer*                m_pCBPerObject;
    VertexBuffer<VertexPosNormTex>    m_vertexBuffer;
    BasicMaterial                     m_material;
};