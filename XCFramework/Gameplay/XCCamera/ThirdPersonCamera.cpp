/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"

#include "ThirdPersonCamera.h"
#include "Gameplay/GameActors/IActor.h"
#include "Gameplay/GameActors/TPCChaseableActor.h"

ThirdPersonCamera::ThirdPersonCamera()
{
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

ThirdPersonCamera::ThirdPersonCamera(IActor* actor, XCVec4& pos, XCVec4& target, XCVec4& up, f32 aspectRatio, f32 fov, f32 nearPlane, f32 farPlane)
    : ICamera(pos, target, up, aspectRatio, fov, nearPlane, farPlane)
{
    m_chaseableActor = actor;

    m_position = pos;
    m_target = target;
    
    m_look = VectorNormalize<3>(target);
    m_up = VectorNormalize<3>(up);
    m_right = VectorCross(m_up, m_look);

    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    BuildViewMatrix();
    BuildProjectionMatrix();
}

void ThirdPersonCamera::Update(f32 dt)
{
    //Get the target to chase position
    if (m_chaseableActor)
    {
        TPCChaseableActor* tpcActor = dynamic_cast<TPCChaseableActor*>(m_chaseableActor);
        if (tpcActor)
        {
            m_target    = m_chaseableActor->GetPosition();
            m_look      = tpcActor->GetLook();
            m_right     = tpcActor->GetRight();
            m_up        = tpcActor->GetUp();
        }

        //Calculate Position
        m_position  = m_target;
        m_position -= (m_distanceFromTarget.Get<Z>() * m_look);
        m_position += (m_distanceFromTarget.Get<X>() * m_right);
        m_position += (m_distanceFromTarget.Get<Y>() * m_up);
    }

    BuildViewMatrix();
}

void ThirdPersonCamera::BuildViewMatrix()
{
    //Build view matrix
    m_look  = VectorNormalize<3>(m_look);

    m_up    = VectorNormalize<3>(VectorCross(m_look, m_right));

    m_right = VectorCross(m_up, m_look);
    
    //Get origin x, y, z
    f32 x = - VectorDot(m_position, m_right);
    f32 y = - VectorDot(m_position, m_up);
    f32 z = - VectorDot(m_position, m_look);

    m_viewMatrix[0][0] = m_right.Get<X>();
    m_viewMatrix[1][0] = m_right.Get<Y>();
    m_viewMatrix[2][0] = m_right.Get<Z>();
    m_viewMatrix[3][0] = x;

    m_viewMatrix[0][1] = m_up.Get<X>();
    m_viewMatrix[1][1] = m_up.Get<Y>();
    m_viewMatrix[2][1] = m_up.Get<Z>();
    m_viewMatrix[3][1] = y;

    m_viewMatrix[0][2] = m_look.Get<X>();
    m_viewMatrix[1][2] = m_look.Get<Y>();
    m_viewMatrix[2][2] = m_look.Get<Z>();
    m_viewMatrix[3][2] = z;

    m_viewMatrix[0][3] = 0.0f;
    m_viewMatrix[1][3] = 0.0f;
    m_viewMatrix[2][3] = 0.0f;
    m_viewMatrix[3][3] = 1;
}

void ThirdPersonCamera::AttachTo(IActor* actor)
{
    m_chaseableActor = actor;
}