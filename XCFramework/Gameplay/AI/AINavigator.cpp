/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "AINavigator.h"

const f32 AINavigator::DISTANCE_ABS          = 5.0f;
const f32 AINavigator::LOOK_TARGET_ANGLE_ABS = 0.2f;

AINavigator::AINavigator(PhysicsActor* bindActor)
{
    m_bindedActor = bindActor;

    //Init the target position to itself
    m_targetPosition = m_bindedActor->GetPosition();
    m_navigateType = NAVIGATE_IDLE;
    m_currentDelta = 0.0f;
}

AINavigator::~AINavigator(void)
{
}

void AINavigator::Update(f32 dt)
{
    m_currentDelta = dt;

    switch (m_navigateType)
    {
        case NAVIGATE_IDLE:
            //Nothing
            break;

        case NAVIGATE_CHASE:
            NavigateByChasing();
            break;

        case NAVIGATE_STATIC_TARGET:
            NavigateByTarget();
            break;

        case NAVIGATE_NONE:
            //Don't navigate, just reset everything
            ResetNavigation();
            break;
    }
}

void AINavigator::ResetNavigation()
{
    m_targetPosition = m_bindedActor->GetPosition();
    m_chaseActor = nullptr;

    m_path = XCFloat4::XCFloat4ZeroVector;

    SetNavigationState(NAVIGATE_IDLE);
}

void AINavigator::NavigateByChasing()
{
    //This can be stopped only when explicitly
    //Get the position of actor in every frame and set as new target
    m_targetPosition = m_chaseActor->GetPosition();

    NavigateByTarget();
}

void AINavigator::NavigateByTarget()
{
    ComputePathToTarget();

    if (!HasArrivedAtTarget())
    {
        if (IsLookingAtTarget())
        {
            //Apply some force in the look direction
            //MoveOnPath();
            MoveOnLook();
        }
    }
    else
    {
        //Navigation complete
        SetNavigationState(NAVIGATE_NONE);
        SetStaticTargetNavigation(m_bindedActor->GetPosition());
    }
}

bool AINavigator::IsLookingAtTarget()
{
    //If the angle between the look and path is within absolute range, then it's looking
    //Calculate the angle
    XCVec4 path = m_path;
    XCVec4 look = m_bindedActor->GetLook();

    //We do not want to consider y component, as the target can have varied y component.
    path.Set<Y>(0.0f);
    look.Set<Y>(0.0f);

    path = VectorNormalize<3>(path);
    look = VectorNormalize<3>(look);

    f32 angle = CosineVectorDot(path, look);

    if (angle <= XC_2PI && angle >= -XC_2PI)
    {
        if (angle <= LOOK_TARGET_ANGLE_ABS && angle >= -LOOK_TARGET_ANGLE_ABS)
        {
            return true;
        }
        else
        {
            RotateTowardsTarget(angle);
        }
    }

    return false;
}

void AINavigator::RotateTowardsTarget(f32 angle)
{
    //TODO: Require computation of angle in actor's local space, the problem is of the inverse matrix that contains existing rotation.
    //and the rotation axis changes accordingly, giving unexpected results.
    
    //Logger("[Angle] %f \n", angle);

    m_bindedActor->Yaw(angle /* m_currentDelta*/, 5.0f);
    m_bindedActor->Pitch(0.0f/* m_currentDelta*/, 5.0f);
}

void AINavigator::MoveOnLook()
{
    m_bindedActor->Walk(10.0f);
}

void AINavigator::MoveOnPath()
{
    XCVec4 pathNormal = VectorNormalize<3>(m_path);

    m_bindedActor->AddForce(pathNormal * 10);
}

void AINavigator::SetStaticTargetNavigation(XCVec4& target)
{
    SetNavigationState(NAVIGATE_STATIC_TARGET);
    m_targetPosition = target;
}

void AINavigator::SetChaseNavigation(PhysicsActor* actor)
{
    if (actor != nullptr)
    {
        SetNavigationState(NAVIGATE_CHASE);
        m_chaseActor = actor;
    }
    else
    {
        SetNavigationState(NAVIGATE_NONE);
    }
}

bool AINavigator::HasArrivedAtTarget()
{
    //Calculate distance of Path vector
    f32 pathDistance = VectorLength<3>(m_path);

    //Logger("[Distance] %f %f %f \n", XMVectorGetX(pathDistance), XMVectorGetY(pathDistance), XMVectorGetZ(pathDistance));

    if (pathDistance < DISTANCE_ABS)
    {
        return true;
    }

    return false;
}

XCVec4 AINavigator::ComputePathToTarget()
{
    //Get current position A and minus from Target Position T
    XCVec4 currentPosition = m_bindedActor->GetPosition();
    m_path = m_targetPosition - currentPosition;

    return m_path;
}