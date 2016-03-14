/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "PCCar.h"
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

PCCar::PCCar(void)
{
}

PCCar::~PCCar(void)
{
}

void PCCar::Init(int actorId)
{
    Car::Init(actorId);

    m_directInput = (DirectInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");
}

void PCCar::PreLoad(const void* fbBuffer)
{
    const FBCar* carBuff = (FBCar*)fbBuffer;
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    Car::PreLoad(XCVec3(carBuff->Position()->x(), carBuff->Position()->y(), carBuff->Position()->z()), (XCMesh*)resMgr.GetResource(carBuff->XCMeshResourceName()->c_str()));

    PhysicsActor::PreLoad(fbBuffer);
}

void PCCar::Load()
{
    XCVec3 distance = XCVec3(0.0f, 7.0f, 25.0f);
    TPCChaseableActor::setDistanceFromTarget(XMLoadFloat3(&distance));
    TPCChaseableActor::SetChasingAxis(IActor::m_look, IActor::m_right, IActor::m_up);

    Car::Load();
}

void PCCar::Update(float dt)
{
    if (getIsControlled())
    {
        if (m_directInput->KeyDown(INPUT_UP))
        {
            AddForce(IActor::m_look * 100);
        }

        if (m_directInput->KeyDown(INPUT_DOWN))
        {
            AddForce(IActor::m_look * -20);
        }

        if (m_directInput->KeyDown(INPUT_LEFT))
        {
            Steer(-dt, -5.0f);
        }

        if (m_directInput->KeyDown(INPUT_RIGHT))
        {
            Steer(dt, 5.0f);
        }
    }

    Integrator(dt);
    ClearForce();

    XCVec3 pos;
    XMStoreFloat3(&pos, m_Position);

#if defined(WIN32)
    m_MTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);
#elif defined(XC_ORBIS)
    m_MTranslation = XMMatrixTranslation(pos.getX(), pos.getY(), pos.getZ());
#endif

    m_currentPosition = m_Position;

    Car::Update(dt);
}

void PCCar::Destroy()
{
    Car::Destroy();
}