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
    
    XCVecIntrinsic4             getDistanceFromTarget()                 { return m_distanceFromTarget;  }
    void                        setDistanceFromTarget(XCVecIntrinsic4 value)   { m_distanceFromTarget = value; }
    
    void                        SetChasingAxis(XCVecIntrinsic4& look, XCVecIntrinsic4& right, XCVecIntrinsic4& up);
    XCVecIntrinsic4             GetLook()                               { return *m_look; }
    XCVecIntrinsic4             GetRight()                              { return *m_right; }
    XCVecIntrinsic4             GetUp()                                 { return *m_up; }

private:
    XCVecIntrinsic4             m_distanceFromTarget;
    XCVecIntrinsic4*            m_look;
    XCVecIntrinsic4*            m_right;
    XCVecIntrinsic4*            m_up;
};