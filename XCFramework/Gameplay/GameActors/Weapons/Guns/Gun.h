/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Input/Directinput.h"

#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Graphics/XC_GraphicsDx11.h"
#include "Graphics/XC_Materials/MaterialTypes.h"

#include "Gameplay/GameActors/SimpleMeshActor.h"
#include "Gameplay/GameActors/SubActor.h"
#include "Gameplay/GameActors/Weapons/Bullets/Bullet.h"

class Gun : public SimpleMeshActor, public SubActor
{
public:
    DECLARE_OBJECT_CREATION(Gun)

    Gun(void);
    virtual ~Gun(void);

    virtual void       PreLoad(IActor* parentActor, XCVec3& initialPosition, std::string pMesh);
    virtual void       Load();
    virtual void       Update(f32 dt);
    virtual void       Draw(RenderContext& renderContext);
    virtual void       Destroy();

    virtual void       UpdateOffsets(f32 dt);
    virtual void       ApplyOffsetRotation();

    void               CheckInput();
    void               UpdateGunRecoil(f32 dt);
    void               ShootBullet(std::string bulletActorType, XCVec3& startPosition, XCVec3& target);
    SubActor*          GetSubActor() { return (SubActor*)this; }

protected:
    ShaderType         m_useShaderType;

    BasicMaterial      m_material;

    XCVec4             m_secondaryLookAxis;
    XCVec4             m_secondaryUpAxis;
    XCVec4             m_secondaryRightAxis;

    u32                m_noOfBullets;
    f32                m_recoilDelta;

    f32                m_recoilMaxTime;
    bool               m_canShootBullet;

    DirectInput*       m_directInput;
};