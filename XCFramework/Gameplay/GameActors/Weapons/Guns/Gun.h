/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Input/Directinput.h"
#include "Graphics/XC_Shaders/XC_VertexFormat.h"
#include "Gameplay/GameActors/SimpleMeshActor.h"
#include "Graphics/XC_GraphicsDx11.h"

#include "Graphics/XC_Materials/MaterialTypes.h"
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
    virtual void       Update(float dt);
    virtual void       Draw(RenderContext& renderContext);
    virtual void       Destroy();

    virtual void       UpdateOffsets(float dt);
    virtual void       ApplyOffsetRotation();

    void               CheckInput();
    void               UpdateGunRecoil(float dt);
    void               ShootBullet(std::string bulletActorType, XCVec3& startPosition, XCVec3& target);
    SubActor*          GetSubActor() { return (SubActor*)this; }

protected:
    ShaderType         m_useShaderType;

    BasicMaterial      m_material;

    XCVec4             m_secondaryLookAxis;
    XCVec4             m_secondaryUpAxis;
    XCVec4             m_secondaryRightAxis;

    unsigned int       m_noOfBullets;
    float              m_recoilDelta;

    float              m_recoilMaxTime;
    bool               m_canShootBullet;

    DirectInput*       m_directInput;
};