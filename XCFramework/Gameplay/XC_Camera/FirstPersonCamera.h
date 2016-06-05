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
    FirstPersonCamera(XCVecIntrinsic4 _pos, XCVecIntrinsic4 _target, XCVecIntrinsic4 _up, float _aspectRatio, float _fov, float _near, float _far);

    virtual ~FirstPersonCamera();

    virtual void            BuildViewMatrix();
    virtual void            Update(float dt);

    void                    Walk(float distance);
    void                    Strafe(float distance);
    void                    Yaw(float angle);
    void                    Pitch(float angle);

private:
    XCVecIntrinsic4         m_look;
    XCVecIntrinsic4         m_right;

    //m_up and m_position is in ICamera
    float                   m_lockYUnit;
    bool                    m_isSetLockY;

    DirectInput*            m_directInput;
};