/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class ICamera
{
public:
    ICamera(void);
    ICamera(XCVecIntrinsic4 _pos, XCVecIntrinsic4 _target, XCVecIntrinsic4 _up, float _aspectRatio, float _fov, float _near, float _far);

    virtual ~ICamera(void);

    virtual void            Init();
    virtual void            Update(float dt);
    virtual void            OnResize(int clientWidth, int clientHeight);
    virtual void            Destroy();

    virtual void            BuildViewMatrix();
    virtual void            BuildProjectionMatrix();

    XCMatrix4               GetViewMatrix() { return m_viewMatrix; }
    XCMatrix4               GetProjectionMatrix() { return m_projectionMatrix; }

    XCVecIntrinsic4         GetTarget() { return m_target; }
    void                    SetTarget(XCVecIntrinsic4 _target) { m_target = _target; }

    XCVecIntrinsic4         GetPosition() { return m_position; }
    void                    SetPosition(XCVecIntrinsic4 _position) { m_position = _position; }

    void                    SetAspectRatio(float _aspectRatio) { m_aspectRatio = _aspectRatio; }
    void                    SetUp(XCVecIntrinsic4 _up) { m_up = _up; }
    void                    SetFOV(float _angle) { m_fov = _angle; }
    void                    SetNearPlane(float _value) { m_nearPlane = _value; }
    void                    SetFarPlane(float _value) { m_farPlane = _value; }

protected:
    float                   m_aspectRatio;
    float                   m_fov;
    float                   m_nearPlane;
    float                   m_farPlane;

    XCVecIntrinsic4         m_target;
    XCVecIntrinsic4         m_position;
    XCVecIntrinsic4         m_up;

    XCMatrix4               m_projectionMatrix;
    XCMatrix4               m_viewMatrix;
};