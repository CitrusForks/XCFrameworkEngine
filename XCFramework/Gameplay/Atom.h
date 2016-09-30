/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "IFeature.h"

class XCGraphics;

//Atom is the basic building block for matter and consists the structural properties of an object placed in Matter
//Combinations of them can work together to provide different working mechanism
class Atom
{
public:
    Atom();
    virtual ~Atom();

    virtual void            Init() {}
    virtual void            Update(f32 dt) {}
    virtual void            Draw(XCGraphics& graphics) {}
    virtual void            Destroy() {}

private:

    std::vector<IFeature*> m_features;
};