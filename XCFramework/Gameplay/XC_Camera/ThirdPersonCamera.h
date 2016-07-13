/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Camera/ICamera.h"

class IActor;

class ThirdPersonCamera : public ICamera
{
public:
    ThirdPersonCamera();
    ThirdPersonCamera(IActor* actor, XCVec4& pos, XCVec4& target, XCVec4& up, float aspectRatio, float fov, float nearPlane, float farPlane);

    virtual ~ThirdPersonCamera();

    virtual void   BuildViewMatrix();
    virtual void   Update(float dt);

    void           AttachTo(IActor* actor);
    void           SetDistanceFromTarget(XCVec4& distance) { m_distanceFromTarget = distance; }

private:
    XCVec4         m_look;
    XCVec4         m_right;

    //m_up and m_position is in ICamera
    IActor*        m_chaseableActor;
    XCVec4         m_distanceFromTarget;
};
