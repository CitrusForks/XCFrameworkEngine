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

    virtual void                    Walk(f32 scalarForce)                 {}
    virtual void                    Jump(f32 scalarForce)                 {}
    virtual void                    Strafe(f32 scalarForce)               {}
    virtual void                    Yaw(f32 angle, f32 scalarForce)     {}
    virtual void                    Pitch(f32 angle, f32 scalarForce)   {}

    virtual void                    Accelerate(f32 dt)                    {}
    virtual void                    Steer(f32 angle, f32 scalarForce)   {}
};