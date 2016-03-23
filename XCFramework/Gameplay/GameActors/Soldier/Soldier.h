/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/PhysicsActor.h"

#include "Engine/Graphics/XC_Materials/MaterialTypes.h"
#include "Gameplay/GameActors/Weapons/Guns/Gun.h"

class Soldier : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Soldier)

    static const float                  MAX_PITCH_ANGLE;

    Soldier(void);
    virtual ~Soldier(void);
 
    virtual void                        PreLoad(XCVec3 initialPosition, XCMesh* pMesh);
    virtual void                        Load();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                SetInitialPhysicsProperties();

    virtual void                        ApplyRotation(XCMatrix4 rotation);

    //Depreciated. Don't use this. Only for testing non axis movement
    void                                AccelerateCar(float dt);

    virtual void                        Walk(float scalarForce);
    virtual void                        Jump(float scalarForce);
    virtual void                        Strafe(float scalarForce);
    virtual void                        Yaw(float angle, float scalarForce);
    virtual void                        Pitch(float angle, float scalarForce);

protected:

    ShaderType                          m_useShaderType;

    BasicMaterial                       m_material;
    bool                                m_isMainCharacter;

    float                               m_totalPitchAngle;
    Gun*                                m_gun;
};