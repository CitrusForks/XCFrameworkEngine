/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Atom.h"

class XC_Graphics;

//Matter is the collection of other child matter and contains interactive Atoms that have some features and provide behavior to matter
class Matter
{
public:
    Matter();
    virtual ~Matter();

    virtual void            Init() {}
    virtual void            Update(f32 dt) {}
    virtual void            Draw(XC_Graphics& graphics) {}
    virtual void            Destroy() {}

private:
    std::vector<Matter*>    m_childMatter;
    std::vector<Atom*>      m_atomsList;
};