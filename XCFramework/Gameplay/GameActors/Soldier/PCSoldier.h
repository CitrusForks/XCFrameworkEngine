/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Soldier/Soldier.h"
#include "Gameplay/GameActors/PlayableCharacterActor.h"
#include "Engine/Input/Directinput.h"

class PCSoldier : public Soldier, public PlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(PCSoldier)

    PCSoldier(void);
    virtual ~PCSoldier(void);

    virtual void                        Init(i32 actorId);
    virtual void                        Update(f32 dt);

protected:
    DirectInput*                        m_directInput;
};