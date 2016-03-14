/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "PCSoldier.h"

PCSoldier::PCSoldier(void)
{
}

PCSoldier::~PCSoldier(void)
{
}

void PCSoldier::Init(int actorId)
{
    Soldier::Init(actorId);

    m_directInput = (DirectInput*) &SystemLocator::GetInstance()->RequestSystem("InputSystem");

    XCVec3 distance = XCVec3(1.5f, 5.0f, 10.0f);
    TPCChaseableActor::setDistanceFromTarget(XMLoadFloat3(&distance));
    TPCChaseableActor::SetChasingAxis(m_secondaryLookAxis, m_secondaryRightAxis, m_secondaryUpAxis);
}

void PCSoldier::PreLoad(const void* fbBuffer)
{
    const FBSoldier* soldierBuff = (FBSoldier*)fbBuffer;
    ResourceManager& resMgr = SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
    Soldier::PreLoad(XCVec3(soldierBuff->Position()->x(), soldierBuff->Position()->y(), soldierBuff->Position()->z()), (XCMesh*)resMgr.GetResource(soldierBuff->XCMeshResourceName()->c_str()));

    PhysicsActor::PreLoad(fbBuffer);
}

void PCSoldier::PreLoad(XCVec3 initialPosition, XCMesh* pMesh)
{
    Soldier::PreLoad(initialPosition, pMesh);
}

void PCSoldier::Update(float dt)
{
    m_gun->CheckInput();

    if (getIsControlled())
    {
        if (m_directInput->KeyDown(INPUT_UP))
        {
            Walk(50);
        }

        if (m_directInput->KeyDown(INPUT_DOWN))
        {
            Walk(-50);
        }

        if (m_directInput->KeyDown(INPUT_LEFT))
        {
            Strafe(-20);
        }

        if (m_directInput->KeyDown(INPUT_RIGHT))
        {
            Strafe(20);
        }

        Yaw(m_directInput->MouseDX() * dt, 5.0f);
        Pitch(m_directInput->MouseDY() * dt, 5.0f);

        if (m_directInput->KeyDown(INPUT_KEY_SPACE))
        {
            Jump(20);
        }
    }

    Integrator(dt);
    ClearForce();

    XCVec3 pos;
    XMStoreFloat3(&pos, m_Position);

#if defined(WIN32)
    m_MTranslation = XMMatrixTranslation(pos.x, pos.y, pos.z);
#else
    m_MTranslation = XMMatrixTranslation(pos.getX(), pos.getY(), pos.getZ());
#endif

    m_currentPosition = m_Position;

    Soldier::Update(dt);
}

void PCSoldier::Destroy()
{
    Soldier::Destroy();
}