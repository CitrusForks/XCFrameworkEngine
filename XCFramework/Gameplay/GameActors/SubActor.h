/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Gameplay/GameActors/IActor.h"

class SubActor
{
public:
    SubActor();
    virtual ~SubActor(void);

    virtual void           Init(IActor* bindedParentActor);
    virtual void           UpdateOffsets(float dt);

    void                   InitOffsets(XCVec4& position, XCVec4& look, XCVec4& right, XCVec4& up);

    XCVec4                 GetOffsetPosition() { return m_offsetPosition; }
    void                   SetOffsetPosition(XCVec4& pos) { m_offsetPosition = pos; }

    XCVec4                 GetOffsetLook() { return m_offsetLook; }
    XCVec4                 GetOffsetRight() { return m_offsetRight; }
    XCVec4                 GetOffsetUp() { return m_offsetUp; }

    void                   SetOffsetLook(XCVec4& look) { m_offsetLook = look; }
    void                   SetOffsetRight(XCVec4& right) { m_offsetRight = right; }
    void                   SetOffsetUp(XCVec4& up) { m_offsetUp = up; }

    void                   SetOffsetRotation(XCMatrix4& rotation) { m_offsetRotation = rotation; }
    XCMatrix4              GetOffsetRotation() { return m_offsetRotation; }

protected:

    XCVec4                 m_offsetLook;
    XCVec4                 m_offsetRight;
    XCVec4                 m_offsetUp;
    XCVec4                 m_offsetPosition;

    XCMatrix4              m_offsetRotation;

    IActor*                m_bindedActor;
};