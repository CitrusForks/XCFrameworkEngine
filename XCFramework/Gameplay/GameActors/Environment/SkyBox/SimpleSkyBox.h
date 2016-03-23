/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/SimpleActor.h"
#include "Engine/Graphics/XC_Textures/CubeTexture3D.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Engine/Graphics/XC_Shaders/XC_RasterizerTypes.h"
#include "Engine/Graphics/VertexBuffer.h"
#include "Engine/Graphics/IndexBuffer.h"

class SimpleSkyBox : public SimpleActor
{
public:
    DECLARE_OBJECT_CREATION(SimpleSkyBox)

    SimpleSkyBox(void);
    virtual ~SimpleSkyBox(void);

    void                            Init(int actorId);
    void                            PreLoad(const void* fbBuffer);
    void                            PreLoad(XCVecIntrinsic4 initialPosition, XCVecIntrinsic4 initialRotation, XCVecIntrinsic4 initialScaling, BasicMaterial material, CubeTexture3D* texture, RasterType rasterType);
    void                            Load();
    void                            Update(float dt);
    void                            Draw(RenderContext& renderContext);
    void                            Destroy();
    void                            BuildBuffers();

protected:
    unsigned int                    m_Stride;
    unsigned int                    m_Offset;

    CubeTexture3D*                  m_cubeMapTexture;

    ShaderType                      m_useShaderType;
    BasicMaterial                   m_material;
    
    VertexBuffer<VertexPos>         m_vertexBuffer;
    IndexBuffer<unsigned int>       m_indexBuffer;

    RasterType                      m_rasterType;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*              m_CBwvp;
#endif
};