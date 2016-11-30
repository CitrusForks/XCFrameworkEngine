/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "NPCCar.h"
#include "Assets/Packages/Schema/BasicTypes_generated.h"
#include "Assets/Packages/Schema/GameplayActors_generated.h"

NPCCar::NPCCar(void)
{
}

NPCCar::~NPCCar(void)
{
}

IActor::ActorReturnState NPCCar::LoadMetaData( const void* metaData )
{
    const FBNPCCar* carBuff = static_cast<const FBNPCCar*>(metaData);
    return Car::LoadMetaData(carBuff->Base());
}
IActor::ActorReturnState NPCCar::Update(f32 dt)
{
    Integrator(dt);
    ClearForce();

    m_MTranslation = MatrixTranslate(m_Position);

    m_currentPosition = m_Position;

    return Car::Update(dt);
}

IActor::ActorReturnState NPCCar::Destroy()
{
    return Car::Destroy();
}