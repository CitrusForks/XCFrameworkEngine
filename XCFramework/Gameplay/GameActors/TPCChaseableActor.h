/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IChaseableActor.h"

//This actor specifies the offset positions with respect to the current Object
class TPCChaseableActor : public IChaseableActor
{
public:
    TPCChaseableActor(void);
    virtual ~TPCChaseableActor(void);
    
    XCVec4             GetDistanceFromTarget()                 { return m_distanceFromTarget;  }
    void               SetDistanceFromTarget(XCVec4& value)    { m_distanceFromTarget = value; }

    void               SetChasingAxis(XCVec4& look, XCVec4& right, XCVec4& up);
    XCVec4             GetLook()                               { return *m_look; }
    XCVec4             GetRight()                              { return *m_right; }
    XCVec4             GetUp()                                 { return *m_up; }

private:
    XCVec4             m_distanceFromTarget;
    XCVec4*            m_look;
    XCVec4*            m_right;
    XCVec4*            m_up;
};