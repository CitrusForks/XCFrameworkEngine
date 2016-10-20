/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "AIBrain.h"

AIBrain::AIBrain(SceneGraph& world, AINavigator* navigator)
{
    m_world = &world;
    m_actionState = ACTIONSTATE_NONE;
    m_bindedNavigator = navigator;
}

AIBrain::~AIBrain(void)
{
}

void AIBrain::Update(f32 dt)
{
    switch (m_actionState)
    {
        case ACTIONSTATE_IDLE:
        {
            //Do nothing, maybe some animation
            m_bindedNavigator->SetNavigationState(NAVIGATE_IDLE);
            break;
        }

        case ACTIONSTATE_WALK:
        {
            //Tell Navigator to move to a specific position, get target here may be from somewhere
            //m_bindedNavigator->SetStaticTargetNavigation(WORLD->GetMainPlayableActor()->GetPosition());
            m_bindedNavigator->SetChaseNavigation((PhysicsActor*)m_world->GetMainPlayableActor());
            SetState(ACTIONSTATE_WALKING);
            break;
        }

        case ACTIONSTATE_WALKING:
        {
            //Tell navigator may be to move to new position
            //Check if we have arrived at the position?
            if (m_bindedNavigator->HasArrivedAtTarget())
            {
                //Go walk again to another position may be
                SetState(ACTIONSTATE_WALK);
            }
            break;
        }

        case ACTIONSTATE_HALT:
            break;

        case  ACTIONSTATE_NONE:
            break;
        default:
                break;
    }
}
