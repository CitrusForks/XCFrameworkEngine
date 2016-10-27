/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/Soldier/Soldier.h"

#include "Engine/GameplayBase/Actors/PlayableCharacterActor.h"

class XCInput;

class PCSoldier : public Soldier, public PlayableCharacterActor
{
public:
    DECLARE_OBJECT_CREATION(PCSoldier)

    PCSoldier(void);
    virtual ~PCSoldier(void);

    void                        Init(i32 actorId);
    void                        PreLoad(const void* fbBuffer) override;
    void                        Update(f32 dt);

protected:
    XCInput*                    m_directInput;
};