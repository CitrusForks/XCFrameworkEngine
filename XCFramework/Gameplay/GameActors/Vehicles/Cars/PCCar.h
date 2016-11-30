/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Vehicles/Cars/Car.h"
#include "Engine/GameplayBase/Actors/PlayableCharacterActor.h"

class XCInput;

class PCCar : public Car, public PlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(PCCar)

    PCCar(void);
    virtual ~PCCar(void);

    IActor::ActorReturnState Init() override;
    IActor::ActorReturnState LoadMetaData(const void* metaData) override;
    IActor::ActorReturnState Load() override;
    IActor::ActorReturnState Update(f32 dt) override;
    IActor::ActorReturnState Destroy() override;

private:
    XCInput*                    m_directInput;
};