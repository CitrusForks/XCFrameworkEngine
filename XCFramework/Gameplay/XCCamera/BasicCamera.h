/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/GameplayBase/Camera/ICamera.h"

class XCInput;

class BasicCamera : public ICamera
{
public:
    BasicCamera(void){}
    BasicCamera(XCVec4& pos, XCVec4& target, XCVec4& up, f32 aspectRatio, f32 fov, f32 nearPlane, f32 farPlane);

    virtual ~BasicCamera(void);

    void            Init();
    void            Update(f32 dt);
    void            Destroy();

    void            BuildViewMatrix();
    void            BuildProjectionMatrix();
    void            SetHeight(f32 dt);
    void            SetRotation();

private:
    f32           m_CameraHeight;
    f32           m_CameraRotationY;
    f32           m_CameraRadius;

    XCInput*    m_directInput;
};