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

    void                        Init(i32 actorId) override;
    void                        PreLoad(const void* fbBuffer) override;
    void                        Load() override;
    void                        Update(f32 dt) override;
    void                        Destroy() override;

private:
    XCInput*                    m_directInput;
};