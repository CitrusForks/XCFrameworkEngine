/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Camera/ICamera.h"

class DirectInput;

class FirstPersonCamera : public ICamera
{
public:
    FirstPersonCamera();
    FirstPersonCamera(XCVec4& pos, XCVec4& target, XCVec4& up, f32 aspectRatio, f32 fov, f32 nearPlane, f32 farPlane);

    virtual ~FirstPersonCamera();

    virtual void            BuildViewMatrix();
    virtual void            Update(f32 dt);

    void                    Walk(f32 distance);
    void                    Strafe(f32 distance);
    void                    Yaw(f32 angle);
    void                    Pitch(f32 angle);

private:

    XCVec4                  m_look;
    XCVec4                  m_right;

    //m_up and m_position is in ICamera
    f32                   m_lockYUnit;
    bool                    m_isSetLockY;

    DirectInput*            m_directInput;
};