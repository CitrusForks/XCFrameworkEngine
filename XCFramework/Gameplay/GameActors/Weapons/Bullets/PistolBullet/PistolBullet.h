/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/EngineUtils.h"
#include "Engine/GameplayBase/Actors/SimpleMeshActor.h"
#include "Engine/GameplayBase/Actors/SubActor.h"

#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCMaterials/MaterialTypes.h"
#include "Graphics/XCMesh/XCMesh.h"

class PistolBullet : public SimpleMeshActor, public SubActor
{
public:
    DECLARE_OBJECT_CREATION(PistolBullet)

    PistolBullet(void);
    ~PistolBullet(void);
    
    IActor::ActorReturnState         Init() override;
    IActor::ActorReturnState         LoadMetaData(const void* metaData) override;
    IActor::ActorReturnState         Update(f32 dt) override;
    void                             UpdateOffsets(f32 dt);
    bool                             Draw(RenderContext& renderContext) override;
    IActor::ActorReturnState         Destroy() override;

    void                             ApplyOffsetRotation();
    SubActor*                        GetSubActor() { return (SubActor*)this; }

protected:

    ShaderType                 m_useShaderType;
    Material                   m_material;
    XCVec4                     m_secondaryLookAxis;
    XCVec4                     m_secondaryUpAxis;
    XCVec4                     m_secondaryRightAxis;
};