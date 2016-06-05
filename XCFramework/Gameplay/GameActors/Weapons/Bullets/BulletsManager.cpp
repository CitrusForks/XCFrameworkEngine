/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"
/*
#include "BulletsManager.h"
#include "Gameplay/GameActors/GameActorsFactory.h"
#include "Engine/Resource/ResourceManager.h"
#include "Gameplay/World.h"

BulletsManager* BulletsManager::m_psBulletsManager = nullptr;

BulletsManager::BulletsManager()
{
    m_worldBulletActorIds.clear();
}

BulletsManager* BulletsManager::GetInstance()
{
    if (m_psBulletsManager == nullptr)
    {
        m_psBulletsManager = new BulletsManager();
    }

    return m_psBulletsManager;
}

void BulletsManager::Update(float dt)
{

}

void BulletsManager::RegisterBullet(Bullet* bulletObj)
{
    m_worldBulletActorIds[bulletObj] = bulletObj->GetActorId();
}

void BulletsManager::UnRegisterBullet(Bullet* bulletObj)
{
    if (bulletObj)
    {
        std::map<Bullet*, int>::iterator it = m_worldBulletActorIds.find(bulletObj);

        if (it != m_worldBulletActorIds.end())
        {
            m_worldBulletActorIds.erase(it);
        }
    }
}

void BulletsManager::DestroyBullet(Bullet* bullet)
{
    if (bullet)
    {
        std::map<Bullet*, int>::iterator it = m_worldBulletActorIds.find(bullet);

        if (it != m_worldBulletActorIds.end())
        {
            int instanceId = it->second;
            WORLD->RequestRemoveActor(instanceId);
        }
    }
}
*/