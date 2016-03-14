/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/GraphicUtils.h"
#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/SimpleMeshActor.h"
#include "Engine/Graphics/XC_Shaders/XC_ShaderManager.h"
#include "Engine/Graphics/XC_GraphicsDx11.h"

#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Engine/Graphics/XC_Mesh/XCMesh.h"
#include "Gameplay/GameActors/SubActor.h"

class PistolBullet : public SimpleMeshActor, public SubActor
{
public:
    DECLARE_OBJECT_CREATION(PistolBullet)

    PistolBullet(void);
    PistolBullet(IActor* parentActor, XCVec3 initialPosition, XCMesh* pMesh);
    
    virtual ~PistolBullet(void);
    
    virtual void                        Init(int actorId);
    virtual void                        BuildGeometryBuffer();
    virtual void                        Update(float dt);
    virtual void                        UpdateOffsets(float dt);
    virtual void                        Draw(RenderContext& renderContext);
    virtual void                        Destroy();
    virtual void                        ApplyOffsetRotation();

    SubActor*                           GetSubActor()                       { return (SubActor*)this;  }

protected:
    SHADERTYPE                        m_useShaderType;

    BasicMaterial                       m_material;

    XCVecIntrinsic4                     m_secondaryLookAxis;
    XCVecIntrinsic4                     m_secondaryUpAxis;
    XCVecIntrinsic4                     m_secondaryRightAxis;

    XCMatrix4                           m_MInitialRotation;

#if defined(XCGRAPHICS_DX12)
    D3DConstantBuffer*                  m_pCBPerObject;
#endif
};