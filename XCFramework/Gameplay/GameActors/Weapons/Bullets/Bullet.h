/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Utils/EngineUtils.h"

#include "Engine/GameplayBase/Actors/PhysicsActor.h"

#include "Graphics/XCMaterials/MaterialTypes.h"
#include "Graphics/XCShaders/XCVertexFormat.h"

class Bullet : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Bullet)

    Bullet(void);
    virtual ~Bullet(void);

    virtual void                        Load();
    virtual void                        SetInitialPhysicsProperties();
    virtual void                        Update(f32 dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                PreLoad(XCVec3& initialPosition, XCVec3& target, std::string pMeshName);
    void                                Shoot(f32 scalarForce);

protected:
    ShaderType                          m_useShaderType;
    Material                            m_material;
    XCVec4                              m_target;
};