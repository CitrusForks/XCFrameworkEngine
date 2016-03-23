/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/PhysicsActor.h"
#include "Engine/Graphics/XC_Materials/MaterialTypes.h"


class Car : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Car)

    Car(void);
    virtual  ~Car(void);

    virtual void                        PreLoad(XCVec3 initialPosition, XCMesh* pMesh);
    virtual void                        Load();
    virtual void                        Update(float dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

    void                                SetInitialPhysicsProperties();
    void                                Accelerate(float dt);
    void                                Steer(float angle, float scalarForce);

protected:
    ShaderType                        m_useShaderType;

    BasicMaterial                       m_material;
    bool                                m_isMainCharacter;
};