/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Camera/ICamera.h"

class DirectInput;

class BasicCamera : public ICamera
{
public:
    BasicCamera(void){}
    BasicCamera(XCVecIntrinsic4 _pos, XCVecIntrinsic4 _target, XCVecIntrinsic4 _up, float _aspectRatio, float _fov, float _near, float _far);

    virtual ~BasicCamera(void);

    void            Init();
    void            Update(float dt);
    void            Destroy();

    void            BuildViewMatrix();
    void            BuildProjectionMatrix();
    void            SetHeight(float dt);
    void            SetRotation();

private:
    float           m_CameraHeight;
    float           m_CameraRotationY;
    float           m_CameraRadius;

    DirectInput*    m_directInput;
};