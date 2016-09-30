/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Matter.h"

class XCGraphics;

//SpaceGraph is a collection of Matter that consumes some physical space.
class SpaceGraph
{
public:
    SpaceGraph();
    virtual ~SpaceGraph();

    virtual void            Init() {}
    virtual void            Update(f32 dt) {}
    virtual void            Draw(XCGraphics& graphics) {}
    virtual void            Destroy() {}

private:
    std::vector<Matter*>    m_matterList;
};