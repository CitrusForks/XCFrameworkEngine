/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"

#include "Engine/Graphics/XC_Camera/ICamera.h"

ICamera::ICamera(void)
{
    m_projectionMatrix = XMMatrixIdentity();
    m_viewMatrix = XMMatrixIdentity();

    m_position  = XMVectorSet(0, 0, 0, 0);
    m_target    = XMVectorSet(0, 0, -1, 0);
    m_up        = XMVectorSet(0, 1, 0, 0);

    m_aspectRatio   = 16/9;
    m_nearPlane     = 1.0f;
    m_farPlane      = 100.0f;
    m_fov           = (float) (0.25 * 3.14);
}

ICamera::ICamera(DirectInput& directInput, XCVecIntrinsic4 _pos, XCVecIntrinsic4 _target, XCVecIntrinsic4 _up, float _aspectRatio, float _fov, float _near, float _far)
    : m_position(_pos)
    , m_target(_target)
    , m_up(_up)
    , m_aspectRatio(_aspectRatio)
    , m_fov(_fov)
    , m_nearPlane(_near)
    , m_farPlane(_far)
{
    XCMatrix4 I =  XMMatrixIdentity();
    m_projectionMatrix = XMMatrixIdentity();
    m_viewMatrix = XMMatrixIdentity();

    BuildViewMatrix();
    BuildProjectionMatrix();

    m_directInput = &directInput;
}

ICamera::~ICamera(void)
{
}

void ICamera::Init()
{

}

void ICamera::Update(float dt)
{
    //Usually View changes in a scene
    BuildViewMatrix();
}

void ICamera::OnResize(int clientWidth, int clientHeight)
{
    m_aspectRatio = (float)(clientWidth / clientHeight);
    BuildProjectionMatrix();
    BuildViewMatrix();
}

void ICamera::BuildProjectionMatrix()
{
    m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void ICamera::BuildViewMatrix()
{
    m_viewMatrix = XMMatrixLookAtLH(m_position, m_target, m_up);
}

void ICamera::Destroy()
{
}
