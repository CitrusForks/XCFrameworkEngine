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

    virtual     void        SetWorld(const XCMatrix4& world) { m_World = world; }
    virtual     XCMatrix4   GetWorld() const { return m_World; }

    virtual     void        ApplyRotation(const XCMatrix4& rotation) { m_MRotation = rotation; }
    virtual     void        ApplyRotation(XCVec4& axis);

    XCVec4                  GetPosition() const { return m_currentPosition; }
    void                    SetPosition(const XCVec4& pos) { m_currentPosition = pos; }

    XCVec4                  GetLook() const { return m_look; }
    XCVec4                  GetRight() const { return m_right; }
    XCVec4                  GetUp() const { return m_up; }

    void                    SetLook(const XCVec4& look) { m_look = look; }
    void                    SetRight(const XCVec4& right) { m_right = right; }
    void                    SetUp(const XCVec4& up) { m_up = up; }

    XCMatrix4               GetMRotation() const { return m_MRotation; }
    void                    SetMRotation(const XCMatrix4& rotation) { m_MRotation = rotation; }

    XCMatrix4               GetMTranslation() const { return m_MTranslation; }
    void                    SetMTranslation(const XCMatrix4& trans) { m_MTranslation = trans; }

    XCMatrix4               GetMScaling() const { return m_MScaling; }
    void                    SetMScaling(const XCMatrix4& trans) { m_MScaling = trans; }

    XCMatrix4               GetTransformedRotation() const { return m_transformedRotation; }
    void                    SetTransformedRotation(const XCMatrix4& rotation) { m_transformedRotation = rotation; }

protected:

    XCMatrix4               m_World;

    XCMatrix4               m_MScaling;
    XCMatrix4               m_MRotation;
    XCMatrix4               m_MTranslation;

    XCVec4                  m_look;
    XCVec4                  m_right;
    XCVec4                  m_up;

    XCVec4                  m_currentPosition;
    XCVec4                  m_initialRotation;
    XCVec4                  m_initialScaling;

    XCMatrix4               m_MInitialRotation;
    XCMatrix4               m_transformedRotation;
};