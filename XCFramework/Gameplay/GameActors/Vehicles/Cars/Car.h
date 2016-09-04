/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/PhysicsActor.h"
#include "Graphics/XC_Materials/MaterialTypes.h"


class Car : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Car)

    Car(void);
    virtual  ~Car(void);

    virtual void        PreLoad(XCVec3& initialPosition, std::string pMesh);
    virtual void        Load() override;
    virtual void        Update(f32 dt) override;
    virtual void        Draw(RenderContext& context) override;
    virtual void        Destroy() override;
                        
    void                SetInitialPhysicsProperties() override;
    void                Accelerate(f32 dt);
    void                Steer(f32 angle, f32 scalarForce);
                        
protected:              
    ShaderType          m_useShaderType;
    Material       m_material;
    bool                m_isMainCharacter;
};