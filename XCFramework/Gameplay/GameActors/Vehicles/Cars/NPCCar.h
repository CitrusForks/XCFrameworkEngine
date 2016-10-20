/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Vehicles/Cars/Car.h"
#include "Gameplay/GameActors/NonPlayableCharacterActor.h"

class NPCCar : public Car, public NonPlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(NPCCar)

    NPCCar(void);
    virtual ~NPCCar(void);
 
    void                        PreLoad(const void* fbBuffer) override;
    void                        Update(f32 dt) override;
    void                        Destroy() override;
};