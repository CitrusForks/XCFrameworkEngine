/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Input/Directinput.h"
#include "Gameplay/GameActors/Vehicles/Cars/Car.h"
#include "Gameplay/GameActors/PlayableCharacterActor.h"

class PCCar : public Car, public PlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(PCCar)

    PCCar(void);
    virtual ~PCCar(void);

    virtual void                        Init(int actorId);
    virtual void                        PreLoad(const void* fbBuffer);
    virtual void                        Load();
    virtual void                        Update(float dt);
    virtual void                        Destroy();

private:
    DirectInput*                        m_directInput;
};