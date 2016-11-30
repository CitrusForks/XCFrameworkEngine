/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Actors/PhysicsActor.h"

#include "Graphics/XCMaterials/MaterialTypes.h"
#include "Graphics/XCShaders/XCShaderTypes.h"
#include "Engine/Input/XCInput.h"

class Door : public PhysicsActor
{
public:
    DECLARE_OBJECT_CREATION(Door)

    Door(void);
    ~Door(void);
    
    virtual IActor::ActorReturnState LoadMetaData(const void* metaData);
    virtual IActor::ActorReturnState Load();
    virtual void                     SetInitialPhysicsProperties();
    virtual IActor::ActorReturnState Update(f32 dt);
    virtual bool                     Draw(RenderContext& renderContext);
    virtual IActor::ActorReturnState Destroy();

protected:
    XCVec3                              m_initialPosition;

    ShaderType                          m_useShaderType;
    XCInput*                            m_directInput;
};