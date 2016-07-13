/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "Engine/GameplayBase/Camera/ICamera.h"

ICamera::ICamera(void)
{
    m_position.SetValues(0, 0, 0, 0);
    m_target.SetValues(0, 0, -1, 0);
    m_up.SetValues(0, 1, 0, 0);

    m_aspectRatio   = 16/9;
    m_nearPlane     = 1.0f;
    m_farPlane      = 100.0f;
    m_fov           = (float) (0.25 * 3.14);
}

ICamera::ICamera(XCVec4& _pos, XCVec4& _target, XCVec4& _up, float _aspectRatio, float _fov, float _near, float _far)
    : m_position(_pos)
    , m_target(_target)
    , m_up(_up)
    , m_aspectRatio(_aspectRatio)
    , m_fov(_fov)
    , m_nearPlane(_near)
    , m_farPlane(_far)
{
    BuildViewMatrix();
    BuildProjectionMatrix();
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
    m_projectionMatrix = MatrixPerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void ICamera::BuildViewMatrix()
{
    m_viewMatrix = MatrixLookAtLH(m_position, m_target, m_up);
}

void ICamera::Destroy()
{
}
