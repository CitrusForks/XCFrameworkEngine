/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class IObjectDimensions
{
public:
    IObjectDimensions();
    virtual ~IObjectDimensions() {}

    virtual             void        SetWorld(XCMatrix4 world) { m_World = world; }
    virtual             XCMatrix4   GetWorld() { return m_World; }

    virtual             void        ApplyRotation(XCMatrix4 rotation) { m_MRotation = rotation; }
    virtual             void        ApplyRotation(XCVecIntrinsic4 axis);

    XCVecIntrinsic4                 GetPosition() { return m_currentPosition; }
    void                            SetPosition(XCVecIntrinsic4 pos) { m_currentPosition = pos; }

    XCVecIntrinsic4                 GetLook() { return m_look; }
    XCVecIntrinsic4                 GetRight() { return m_right; }
    XCVecIntrinsic4                 GetUp() { return m_up; }

    void                            SetLook(XCVecIntrinsic4 look) { m_look = look; }
    void                            SetRight(XCVecIntrinsic4 right) { m_right = right; }
    void                            SetUp(XCVecIntrinsic4 up) { m_up = up; }

    XCMatrix4                       GetMRotation() { return m_MRotation; }
    void                            SetMRotation(XCMatrix4 rotation) { m_MRotation = rotation; }

    XCMatrix4                       GetMTranslation() { return m_MTranslation; }
    void                            SetMTranslation(XCMatrix4 trans) { m_MTranslation = trans; }

    XCMatrix4                       GetMScaling() { return m_MScaling; }
    void                            SetMScaling(XCMatrix4 trans) { m_MScaling = trans; }

    XCMatrix4                       GetTransformedRotation() { return m_transformedRotation; }
    void                            SetTransformedRotation(XCMatrix4 rotation) { m_transformedRotation = rotation; }

protected:
    XCMatrix4                        m_World;

    XCMatrix4                        m_MScaling;
    XCMatrix4                        m_MRotation;
    XCMatrix4                        m_MTranslation;

    XCVecIntrinsic4                  m_look;
    XCVecIntrinsic4                  m_right;
    XCVecIntrinsic4                  m_up;

    XCVecIntrinsic4                  m_currentPosition;
    XCVecIntrinsic4                  m_initialRotation;
    XCVecIntrinsic4                  m_initialScaling;

    XCMatrix4                        m_MInitialRotation;
    XCMatrix4                        m_transformedRotation;
};