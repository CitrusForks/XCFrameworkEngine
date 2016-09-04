/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/PhysicsActor.h"

#include "Graphics/XC_Materials/MaterialTypes.h"
#include "Graphics/XC_Shaders/XC_ShaderTypes.h"
#include "Engine/Input/Directinput.h"

class Door : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Door)

    Door(void);
    virtual ~Door(void);
    
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        PreLoad(XCVec3& _initialPosition, std::string pMesh);
    virtual void                        Load();
    virtual void                        SetInitialPhysicsProperties();
    virtual void                        Update(f32 dt);
    virtual void                        Draw(RenderContext& context);
    virtual void                        Destroy();

protected:
    XCVec3                              m_initialPosition;

    ShaderType                          m_useShaderType;

    Material                       m_material;
    DirectInput*                        m_directInput;
};