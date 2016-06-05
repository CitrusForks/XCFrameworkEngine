/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GameplayPrecompiledHeader.h"
#include "FirstPersonCamera.h"

#include "Engine/Input/Directinput.h"

FirstPersonCamera::FirstPersonCamera()
{
}


FirstPersonCamera::~FirstPersonCamera()
{
}

FirstPersonCamera::FirstPersonCamera(XCVecIntrinsic4 pos, XCVecIntrinsic4 target, XCVecIntrinsic4 up, float aspectRatio, float fov, float nearPlane, float farPlane)
    : ICamera(pos, target, up, aspectRatio, fov, nearPlane, farPlane)
{
    m_isSetLockY = false;
    m_lockYUnit = 0.0f;
    
    m_position = pos;
    m_target = target;
    
    m_look = XMVector3Normalize(target);
    m_up = XMVector3Normalize(up);
    m_right = XMVector3Cross(m_up, m_look);
    
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    
    m_directInput = &SystemLocator::GetInstance()->RequestSystem<DirectInput>("InputSystem");

    BuildViewMatrix();
    BuildProjectionMatrix();
}

void FirstPersonCamera::Update(float dt)
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
    m_look = XMVector3Normalize(m_look);
    
    m_up = XMVector3Normalize(XMVector3Cross(m_look, m_right));
    
    m_right = XMVector3Cross(m_up, m_look);
    
    //Get origin x, y, z
    float x = -XMVectorGetX(XMVector3Dot(m_position, m_right));
    float y = -XMVectorGetY(XMVector3Dot(m_position, m_up));
    float z = -XMVectorGetZ(XMVector3Dot(m_position, m_look));
    
#if defined(WIN32)
    m_viewMatrix._11 = XMVectorGetX(m_right);
    m_viewMatrix._21 = XMVectorGetY(m_right);
    m_viewMatrix._31 = XMVectorGetZ(m_right);
    m_viewMatrix._41 = x;
    
    m_viewMatrix._12 = XMVectorGetX(m_up);
    m_viewMatrix._22 = XMVectorGetY(m_up);
    m_viewMatrix._32 = XMVectorGetZ(m_up);
    m_viewMatrix._42 = y;
    
    m_viewMatrix._13 = XMVectorGetX(m_look);
    m_viewMatrix._23 = XMVectorGetY(m_look);
    m_viewMatrix._33 = XMVectorGetZ(m_look);
    m_viewMatrix._43 = z;
    
    m_viewMatrix._14 = 0.0f;
    m_viewMatrix._24 = 0.0f;
    m_viewMatrix._34 = 0.0f;
    m_viewMatrix._44 = 1;
#elif defined(XC_ORBIS)
    m_viewMatrix.setElem(0, 0, XMVectorGetX(m_right));
    m_viewMatrix.setElem(1, 0, XMVectorGetY(m_right));
    m_viewMatrix.setElem(2, 0, XMVectorGetZ(m_right));
    m_viewMatrix.setElem(3, 0, x);

    m_viewMatrix.setElem(0, 1,  XMVectorGetX(m_up));
    m_viewMatrix.setElem(1, 1,  XMVectorGetY(m_up));
    m_viewMatrix.setElem(2, 1,  XMVectorGetZ(m_up));
    m_viewMatrix.setElem(3, 1,  y);

    m_viewMatrix.setElem(0, 2, XMVectorGetX(m_look));
    m_viewMatrix.setElem(1, 2, XMVectorGetY(m_look));
    m_viewMatrix.setElem(2, 2, XMVectorGetZ(m_look));
    m_viewMatrix.setElem(3, 2, z);

    m_viewMatrix.setElem(0, 3, 0.0f);
    m_viewMatrix.setElem(1, 3, 0.0f);
    m_viewMatrix.setElem(2, 3, 0.0f);
    m_viewMatrix.setElem(3, 3, 1);
#endif
}

void FirstPersonCamera::Walk(float distance)
{
    m_position += (distance * m_look);
    
    if (m_isSetLockY)
    {
        m_position = XMVectorSetY(m_position, m_lockYUnit);
    }
}

void FirstPersonCamera::Strafe(float distance)
{
    m_position += (distance * m_right);
}

void FirstPersonCamera::Yaw(float angle)
{
    XCMatrix4 rotation = XMMatrixRotationY(angle);
    
    m_right = XMVector3TransformNormal(m_right, rotation);
    m_up = XMVector3TransformNormal(m_up, rotation);
    m_look = XMVector3TransformNormal(m_look, rotation);
}

void FirstPersonCamera::Pitch(float angle)
{
    XCMatrix4 rotation = XMMatrixRotationAxis(m_right, angle);
    
    m_up = XMVector3TransformNormal(m_up, rotation);
    m_look = XMVector3TransformNormal(m_look, rotation);
}