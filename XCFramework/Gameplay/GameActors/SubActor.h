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

    virtual void                    Init(IActor* bindedParentActor);
    void                            InitOffsets(XCVecIntrinsic4 position, XCVecIntrinsic4 look, XCVecIntrinsic4 right, XCVecIntrinsic4 up);
    virtual void                    UpdateOffsets(float dt);

    XCVecIntrinsic4                 GetOffsetPosition()                      { return m_offsetPosition; }
    void                            SetOffsetPosition(XCVecIntrinsic4 pos)   { m_offsetPosition = pos; }

    XCVecIntrinsic4                 GetOffsetLook()                          { return m_offsetLook; }
    XCVecIntrinsic4                 GetOffsetRight()                         { return m_offsetRight; }
    XCVecIntrinsic4                 GetOffsetUp()                            { return m_offsetUp; }

    void                            SetOffsetLook(XCVecIntrinsic4 look)      { m_offsetLook = look; }
    void                            SetOffsetRight(XCVecIntrinsic4 right)    { m_offsetRight = right; }
    void                            SetOffsetUp(XCVecIntrinsic4 up)          { m_offsetUp = up; }

    void                            SetOffsetRotation(XCMatrix4 rotation)    { m_offsetRotation = rotation; }
    XCMatrix4                       GetOffsetRotation()                      { return m_offsetRotation;     }
protected:
    XCVecIntrinsic4                 m_offsetLook;
    XCVecIntrinsic4                 m_offsetRight;
    XCVecIntrinsic4                 m_offsetUp;
    XCVecIntrinsic4                 m_offsetPosition;

    XCMatrix4                       m_offsetRotation;

    IActor*                         m_bindedActor;
};