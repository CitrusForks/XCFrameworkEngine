/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

class XCGraphics;

//IFeature is a interface that every Atom can have as many as can.
class IFeature
{
public:
    IFeature();
    virtual ~IFeature();

    virtual void            Init() {}
    virtual void            Update(f32 dt) {}
    virtual void            Draw(XCGraphics& graphics) {}
    virtual void            Destroy() {}

private:
};