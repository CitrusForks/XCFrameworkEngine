/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Graphics/XCShaders/XCVertexFormat.h"
#include "Engine/GameplayBase/Actors/PhysicsActor.h"
#include "Graphics/XCMaterials/MaterialTypes.h"


class Car : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Car)

    Car(void);
    virtual  ~Car(void);

    virtual void        PreLoad(const void* fbBuffer) override;
    virtual void        Load() override;
    virtual void        Update(f32 dt) override;
    virtual void        Draw(RenderContext& context) override;
    virtual void        Destroy() override;
                        
    void                SetInitialPhysicsProperties() override;
    void                Accelerate(f32 dt);
    void                Steer(f32 angle, f32 scalarForce);
                        
protected:              
    ShaderType          m_useShaderType;
    Material            m_material;
    bool                m_isMainCharacter;
};