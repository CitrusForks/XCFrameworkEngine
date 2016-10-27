/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

enum ENavigationType
{
    NAVIGATE_IDLE,
    NAVIGATE_CHASE,
    NAVIGATE_STATIC_TARGET,

    NAVIGATE_NONE
};

class PhysicsActor;

class AINavigator
{
public:
    static const f32 DISTANCE_ABS;
    static const f32 LOOK_TARGET_ANGLE_ABS;

    AINavigator(PhysicsActor* actor);
    virtual ~AINavigator(void);

    void                      SetNavigationState(ENavigationType type) { m_navigateType = type; }
    void                      SetStaticTargetNavigation(XCVec4& target);
    void                      SetChaseNavigation(PhysicsActor* actor);

    XCVec4                    GetTarget() { return m_targetPosition; }

    void                      Update(f32 dt);

    void                      RotateTowardsTarget(f32 angle);
    bool                      IsLookingAtTarget();
    void                      MoveOnLook();
    void                      MoveOnPath();
    bool                      HasArrivedAtTarget();

    XCVec4                    ComputePathToTarget();

    void                      ResetNavigation();

protected:
    void                      NavigateByChasing();
    void                      NavigateByTarget();

private:
    XCVec4                    m_path;
    XCVec4                    m_targetPosition;

    PhysicsActor*             m_bindedActor;
    PhysicsActor*             m_chaseActor;
                              
    ENavigationType           m_navigateType;
    f32                       m_currentDelta;
};