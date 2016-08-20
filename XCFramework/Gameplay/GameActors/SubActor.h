/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class IActor;

class SubActor
{
public:
    SubActor();
    virtual ~SubActor(void);

    virtual void           Init(const IActor* bindedParentActor);
    virtual void           UpdateOffsets(f32 dt);

    void                   InitOffsets(const XCVec4& position, const XCVec4& look, const XCVec4& right, const XCVec4& up);

    XCVec4                 GetOffsetPosition() { return m_offsetPosition; }
    void                   SetOffsetPosition(const XCVec4& pos) { m_offsetPosition = pos; }

    XCVec4                 GetOffsetLook() { return m_offsetLook; }
    XCVec4                 GetOffsetRight() { return m_offsetRight; }
    XCVec4                 GetOffsetUp() { return m_offsetUp; }

    void                   SetOffsetLook(const XCVec4& look) { m_offsetLook = look; }
    void                   SetOffsetRight(const XCVec4& right) { m_offsetRight = right; }
    void                   SetOffsetUp(const XCVec4& up) { m_offsetUp = up; }

    void                   SetOffsetRotation(const XCMatrix4& rotation) { m_offsetRotation = rotation; }
    XCMatrix4              GetOffsetRotation() { return m_offsetRotation; }

protected:

    XCVec4                 m_offsetLook;
    XCVec4                 m_offsetRight;
    XCVec4                 m_offsetUp;
    XCVec4                 m_offsetPosition;

    XCMatrix4              m_offsetRotation;

    const IActor*          m_bindedActor;
};