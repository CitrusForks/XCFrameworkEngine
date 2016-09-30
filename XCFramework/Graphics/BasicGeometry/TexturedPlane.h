/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Plane.h"

#include "Engine/ObjectBase/IObjectDimensions.h"

#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCShaders/XCShaderTypes.h"
#include "Graphics/XCMaterials/MaterialTypes.h"
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
    virtual void                     PreLoad(XCVec4& initialPosition, XCVec4& initialRotation, XCVec4& initialScaling, Material& material, std::string texture, RasterType rasterType);
    virtual void                     Load();
    virtual void                     Update(f32 dt);
    virtual void                     Draw(RenderContext& renderContext);
    virtual void                     Destroy();
    virtual void                     BuildGeometryBuffers();

protected:

    ShaderType                        m_useShaderType;
    ResourceHandle*                   m_texture;
    GPUResource*                      m_pCBPerObject;
    VertexBuffer<VertexPosNormTex>    m_vertexBuffer;
    Material                          m_material;
};