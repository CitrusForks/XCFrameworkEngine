/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

//All the navigations of an Actor will be put in this interface, so that any AI/User input can command
//these navigations.

class INavigator
{
public:
    INavigator(void);
    virtual ~INavigator(void);

    virtual void                    Walk(float scalarForce)                 {}
    virtual void                    Jump(float scalarForce)                 {}
    virtual void                    Strafe(float scalarForce)               {}
    virtual void                    Yaw(float angle, float scalarForce)     {}
    virtual void                    Pitch(float angle, float scalarForce)   {}

    virtual void                    Accelerate(float dt)                    {}
    virtual void                    Steer(float angle, float scalarForce)   {}
};