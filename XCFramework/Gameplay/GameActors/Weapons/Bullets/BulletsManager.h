/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

/*
#include "Gameplay/GameActors/Weapons/Bullets/Bullet.h"

class BulletsManager
{
public:
    static BulletsManager*                 GetInstance();

    void                                   Update(float dt);
    void                                   ShootBullet(std::string bulletActorType, XMFLOAT3 startPosition, XMFLOAT3 target);
    void                                   DestroyBullet(Bullet* bullet);
    void                                   CheckBulletHit();

    void                                   RegisterBullet(Bullet* bulletObj);
    void                                   UnRegisterBullet(Bullet* bulletObj);

private:
    BulletsManager();

    static BulletsManager*                 m_psBulletsManager;

    std::map<Bullet*, int>                 m_worldBulletActorIds;
};

#define BULLETS_MANAGER                    BulletsManager::GetInstance()
*/