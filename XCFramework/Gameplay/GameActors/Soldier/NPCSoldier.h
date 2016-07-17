/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Soldier/Soldier.h"
#include "Gameplay/GameActors/NonPlayableCharacterActor.h"
#include "Gameplay/AI/AIBrain.h"
#include "Gameplay/AI/AINavigator.h"

class NPCSoldier : public Soldier, public NonPlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(NPCSoldier)

    NPCSoldier(void);
    virtual ~NPCSoldier(void);

    virtual void                        Init(i32 actorId);
    virtual void                        Update(f32 dt);

protected:
    std::unique_ptr<AIBrain>            m_AIBrain;
    std::unique_ptr<AINavigator>        m_AINavigator;
};