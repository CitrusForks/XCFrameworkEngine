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
    ThirdPersonCamera(IActor* actor, XCVecIntrinsic4 _pos, XCVecIntrinsic4 _target, XCVecIntrinsic4 _up, float _aspectRatio, float _fov, float _near, float _far);

    virtual ~ThirdPersonCamera();

    virtual void            BuildViewMatrix();
    virtual void            Update(float dt);

    void                    AttachTo(IActor* actor);
    void                    SetDistanceFromTarget(XCVecIntrinsic4 distance)    { m_distanceFromTarget = distance;  }

private:
    XCVecIntrinsic4         m_look;
    XCVecIntrinsic4         m_right;

    //m_up and m_position is in ICamera
    IActor*                 m_chaseableActor;
    XCVecIntrinsic4         m_distanceFromTarget;
};
