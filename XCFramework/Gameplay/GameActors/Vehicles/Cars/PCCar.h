/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Input/XCInput.h"
#include "Gameplay/GameActors/Vehicles/Cars/Car.h"
#include "Gameplay/GameActors/PlayableCharacterActor.h"

class PCCar : public Car, public PlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(PCCar)

    PCCar(void);
    virtual ~PCCar(void);

    virtual void                        Init(i32 actorId);
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        Load();
    virtual void                        Update(f32 dt);
    virtual void                        Destroy();

private:
    XCInput*                        m_directInput;
};