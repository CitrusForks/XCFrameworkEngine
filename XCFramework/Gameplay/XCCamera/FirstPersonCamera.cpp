/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"
#include "FirstPersonCamera.h"

#include "Engine/Input/XCInput.h"

FirstPersonCamera::FirstPersonCamera()
{
}


FirstPersonCamera::~FirstPersonCamera()
{
}

FirstPersonCamera::FirstPersonCamera(XCVec4& pos, XCVec4& target, XCVec4& up, f32 aspectRatio, f32 fov, f32 nearPlane, f32 farPlane)
    : ICamera(pos, target, up, aspectRatio, fov, nearPlane, farPlane)
{
    m_isSetLockY = false;
    m_lockYUnit = 0.0f;
    
    m_position = pos;
    m_target = target;
    
    m_look  = VectorNormalize<3>(target);
    m_up    = VectorNormalize<3>(up);
    m_right = VectorCross(m_up, m_look);
    
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    
    m_directInput = &SystemLocator::GetInstance()->RequestSystem<XCInput>("InputSystem");

    BuildViewMatrix();
    BuildProjectionMatrix();
}

void FirstPersonCamera::Update(f32 dt)
{
    //Check for inputs
    if (m_directInput->KeyDown(INPUT_UP))
    {
        Walk(dt * 10);
    }
    if (m_directInput->KeyDown(INPUT_DOWN))
    {
        Walk(-dt * 10);
    }
    if (m_directInput->KeyDown(INPUT_LEFT))
    {
        Strafe(-dt);
    }
    if (m_directInput->KeyDown(INPUT_RIGHT))
    {
        Strafe(dt);
    }

    Yaw(m_directInput->MouseDX() / 25.0f);
    Pitch(m_directInput->MouseDY() / 25.0f);

    BuildViewMatrix();
}

void FirstPersonCamera::BuildViewMatrix()
{
    //Build view matrix
    m_look = VectorNormalize<3>(m_look);
    
    m_up = VectorNormalize<3>(VectorCross(m_look, m_right));
    
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

void FirstPersonCamera::Walk(f32 distance)
{
    m_position += (distance * m_look);
    
    if (m_isSetLockY)
    {
        m_position.Set<Y>(m_lockYUnit);
    }
}

void FirstPersonCamera::Strafe(f32 distance)
{
    m_position += (distance * m_right);
}

void FirstPersonCamera::Yaw(f32 angle)
{
    XCMatrix4 rotation = MatrixRotationY(angle);
    
    m_right = VectorTransformNormal(m_right, rotation);
    m_up    = VectorTransformNormal(m_up, rotation);
    m_look  = VectorTransformNormal(m_look, rotation);
}

void FirstPersonCamera::Pitch(f32 angle)
{
    XCMatrix4 rotation = MatrixRotationAxis(m_right, angle);
    
    m_up    = VectorTransformNormal(m_up, rotation);
    m_look  = VectorTransformNormal(m_look, rotation);
}