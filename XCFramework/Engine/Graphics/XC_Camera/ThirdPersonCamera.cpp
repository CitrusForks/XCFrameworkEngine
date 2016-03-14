/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "ThirdPersonCamera.h"
#include "Gameplay/GameActors/TPCChaseableActor.h"


ThirdPersonCamera::ThirdPersonCamera()
{
}


ThirdPersonCamera::~ThirdPersonCamera()
{
}

ThirdPersonCamera::ThirdPersonCamera(DirectInput& input, IActor* actor, XCVecIntrinsic4 pos, XCVecIntrinsic4 target, XCVecIntrinsic4 up, float aspectRatio, float fov, float nearPlane, float farPlane)
    : ICamera(input, pos, target, up, aspectRatio, fov, nearPlane, farPlane)
{
    m_chaseableActor = actor;

    m_position = pos;
    m_target = target;
    
    m_look = XMVector3Normalize(target);
    m_up = XMVector3Normalize(up);
    m_right = XMVector3Cross(m_up, m_look);

    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;

    BuildViewMatrix();
    BuildProjectionMatrix();
}

void ThirdPersonCamera::Update(float dt)
{
    //Get the target to chase position
    if (m_chaseableActor)
    {
        TPCChaseableActor* tpcActor = dynamic_cast<TPCChaseableActor*>(m_chaseableActor);
        if (tpcActor)
        {
            m_target = m_chaseableActor->GetPosition();
            m_look = tpcActor->GetLook();
            m_right = tpcActor->GetRight();
            m_up = tpcActor->GetUp();
        }

        //Calculate Position
        m_position = m_target;
        m_position -= (XMVectorGetZ(m_distanceFromTarget) * m_look);
        m_position += (XMVectorGetX(m_distanceFromTarget) * m_right);
        m_position += (XMVectorGetY(m_distanceFromTarget) * m_up);
    }

    BuildViewMatrix();
}

void ThirdPersonCamera::BuildViewMatrix()
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

    m_viewMatrix.setElem(0, 1, XMVectorGetX(m_up));
    m_viewMatrix.setElem(1, 1, XMVectorGetY(m_up));
    m_viewMatrix.setElem(2, 1, XMVectorGetZ(m_up));
    m_viewMatrix.setElem(3, 1, y);

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

void ThirdPersonCamera::AttachTo(IActor* actor)
{
    m_chaseableActor = actor;
}