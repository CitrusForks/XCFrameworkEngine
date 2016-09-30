/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "PCCar.h"
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

PCCar::PCCar(void)
{
}

PCCar::~PCCar(void)
{
}

void PCCar::Init(i32 actorId)
{
    Car::Init(actorId);

    m_directInput = (XCInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");
}

void PCCar::PreLoad(const void* fbBuffer)
{
    const FBCar* carBuff = (FBCar*)fbBuffer;

    Car::PreLoad(XCVec3(carBuff->Position()->x(), carBuff->Position()->y(), carBuff->Position()->z()), carBuff->XCMeshResourceName()->c_str());
    PhysicsActor::PreLoad(fbBuffer);
}

void PCCar::Load()
{
    XCVec4 distance(0.0f, 7.0f, 25.0f, 0.0f);
    TPCChaseableActor::SetDistanceFromTarget(distance);
    TPCChaseableActor::SetChasingAxis(IActor::m_look, IActor::m_right, IActor::m_up);

    Car::Load();
}

void PCCar::Update(f32 dt)
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

    m_MTranslation = MatrixTranslate(m_Position);

    m_currentPosition = m_Position;

    Car::Update(dt);
}

void PCCar::Destroy()
{
    Car::Destroy();
}