/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Soldier/Soldier.h"


#include "Engine/GameplayBase/Actors/NonPlayableCharacterActor.h"

class AIBrain;
class AINavigator;

class NPCSoldier : public Soldier, public NonPlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(NPCSoldier)

    NPCSoldier(void);
    virtual ~NPCSoldier(void);

    void                                Init(i32 actorId) override;
    void                                PreLoad(const void* fbBuffer) override;
    void                                Update(f32 dt) override;

protected:
    std::unique_ptr<AIBrain>            m_AIBrain;
    std::unique_ptr<AINavigator>        m_AINavigator;
};