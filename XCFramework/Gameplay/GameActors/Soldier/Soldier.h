/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Graphics/XCMaterials/MaterialTypes.h"

#include "Engine/GameplayBase/Actors/PhysicsActor.h"
#include "Gameplay/GameActors/Weapons/Guns/Gun.h"

class Soldier : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Soldier)

    static const f32                  MAX_PITCH_ANGLE;

    Soldier(void);
    virtual ~Soldier(void);
 
    virtual IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    virtual IActor::ActorReturnState Load() override;
    virtual IActor::ActorReturnState OnLoaded() override;
    virtual IActor::ActorReturnState Update(f32 dt) override;
    virtual bool                     Draw(RenderContext& renderContext) override;
    virtual IActor::ActorReturnState Destroy() override;

    virtual void             ApplyRotation(XCMatrix4& rotation);

    void                     SetInitialPhysicsProperties()   override final;

    //Depreciated. Don't use this. Only for testing non axis movement
    void                     AccelerateCar(f32 dt);

    virtual void             Walk(f32 scalarForce);
    virtual void             Jump(f32 scalarForce);
    virtual void             Strafe(f32 scalarForce);
    virtual void             Yaw(f32 angle, f32 scalarForce);
    virtual void             Pitch(f32 angle, f32 scalarForce);

protected:

    ShaderType               m_useShaderType;

    Material                 m_material;
    bool                     m_isMainCharacter;

    f32                      m_totalPitchAngle;
    Gun*                     m_gun;
};