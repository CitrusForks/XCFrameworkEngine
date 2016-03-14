/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/GraphicUtils.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/SimpleMeshActor.h"

#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Gameplay/GameActors/PhysicsActor.h"


class Bullet : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Bullet)

    Bullet(void);
    virtual ~Bullet(void);

    virtual void                        PreLoad(XCVec3 initialPosition, XCVec3 target, XCMesh* pMesh);
    virtual void                        Load();
    virtual void                        BuildMeshBuffer();
    virtual void                        SetInitialPhysicsProperties();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    virtual void                        Shoot(float scalarForce);

protected:
    SHADERTYPE                        m_useShaderType;

    Texture2D*                          m_texture;
    BasicMaterial                       m_material;

    XCMatrix4                           m_MInitialRotation;
    XCVecIntrinsic4                     m_target;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*                  m_pCBPerObject;
#endif
};