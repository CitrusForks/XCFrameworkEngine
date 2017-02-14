/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "PCCar.h"
#include "Engine/Input/XCInput.h"

#include "Physics/IPhysicsFeature.h"
#include "Physics/Phusike/RigidBody.h"

PCCar::PCCar(void)
{
}

PCCar::~PCCar(void)
{
}

IActor::ActorReturnState PCCar::Init()
{
    m_directInput = (XCInput*)&SystemLocator::GetInstance()->RequestSystem("InputSystem");

    return Car::Init();
}

IActor::ActorReturnState PCCar::LoadMetaData( const void* metaData )
{
    const FBPCCar* carBuff = static_cast<const FBPCCar*>(metaData);
    return Car::LoadMetaData(carBuff->Base());
}

IActor::ActorReturnState PCCar::Load()
{
    XCVec4 distance(0.0f, 7.0f, 25.0f, 0.0f);
    TPCChaseableActor::SetDistanceFromTarget(distance);
    TPCChaseableActor::SetChasingAxis(IActor::m_look, IActor::m_right, IActor::m_up);

    return Car::Load();
}

IActor::ActorReturnState PCCar::Update(f32 dt)
{
    if (getIsControlled())
    {
        if (m_directInput->KeyDown(INPUT_UP))
        {
            m_physicsFeature->GetTyped<RigidBody>()->AddForce(IActor::m_look * 100);
        }

        if (m_directInput->KeyDown(INPUT_DOWN))
        {
            m_physicsFeature->GetTyped<RigidBody>()->AddForce(IActor::m_look * -20);
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

    if(m_physicsFeature->IsDirty())
    {
        m_currentPosition = m_physicsFeature->GetTransformedPosition();

        m_MTranslation = MatrixTranslate(m_currentPosition);
    }

    return Car::Update(dt);
}

IActor::ActorReturnState PCCar::Destroy()
{
    return Car::Destroy();
}