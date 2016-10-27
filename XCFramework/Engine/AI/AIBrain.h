/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "AINavigator.h"

#include "Engine/GameplayBase/SceneGraph.h"

enum EActionState
{
    ACTIONSTATE_IDLE,

    ACTIONSTATE_WALK,
    ACTIONSTATE_WALKING,
    ACTIONSTATE_HALT,

    ACTIONSTATE_NONE
};

class AIBrain
{
public:
    AIBrain(SceneGraph& world, AINavigator* navigator);
    ~AIBrain(void);

    void virtual                        Update(f32 dt);

    void                                SetState(EActionState state) { m_actionState = state; }

private:
    EActionState                        m_actionState;
    AINavigator*                        m_bindedNavigator;
    SceneGraph*                         m_world;
};